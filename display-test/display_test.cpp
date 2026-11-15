#include <iostream>
#include <vector>
#include <thread>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <libconfig.h++>

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <array>
#include <cerrno>
#include <stdexcept>

#include "../display-state/DisplayState.h"
#include "../display-test/KeypadDecoder.h"

struct SerialConfig {
    std::string port;
    int baud_rate;
    int data_bits;
    std::string parity;
    int stop_bits;
    int send_interval_ms;
};

speed_t getBaudRate(int baud) {
    switch (baud) {
        case 19200: return B19200;
        case 9600:  return B9600;
        case 115200: return B115200;
        default: throw std::invalid_argument("Unsupported baud rate");
    }
}

void sendData(int serialPort, DisplayState& displaystate, int interval_ms) {
    auto nextTimePoint = std::chrono::steady_clock::now();
    std::vector<uint8_t> data;

    while (true) {
        data = displaystate.serialize();
        ssize_t written = write(serialPort, data.data(), data.size());
        if (written == -1) {
            perror("Error writing to serial port");
            break;
        }

        // Schedule the next transmission
        nextTimePoint += std::chrono::milliseconds(interval_ms);

        // Wait until the next scheduled time
        std::this_thread::sleep_until(nextTimePoint);
    }
}

void readData(int serialPort) {
    constexpr uint8_t SYNC_BYTE = 0x80;
    constexpr size_t FRAME_SIZE = sizeof(InputFrame);

    InputFrame prevFrame{};
    InputFrame currFrame{};
    // Read directly into the current frame's object representation. The
    // static_asserts above guarantee that its byte layout matches the wire.
    auto* currBytes = reinterpret_cast<unsigned char*>(&currFrame);
    size_t buffered = 0;
    bool havePrevFrame = false;

    while (true) {
        ssize_t bytesRead = read(
            serialPort,
            currBytes + buffered,
            FRAME_SIZE - buffered
        );

        if (bytesRead < 0) {
            if (errno == EINTR)
                continue;

            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Use poll() for non-blocking descriptors.
                continue;
            }

            perror("Serial read error");
            return;
        }

        if (bytesRead == 0)
            continue;

        buffered += static_cast<size_t>(bytesRead);
        if (buffered < FRAME_SIZE)
            continue;

        // Locate the sync byte in the received frame bytes. If it is not at
        // the start, shift the candidate frame bytes to the front.
        size_t syncPos = 0;
        while (syncPos < FRAME_SIZE && currBytes[syncPos] != SYNC_BYTE)
            ++syncPos;

        if (syncPos == FRAME_SIZE) {
            buffered = 0;
            continue;
        }

        if (syncPos != 0) {
            const size_t remaining = FRAME_SIZE - syncPos;
            std::memmove(currBytes, currBytes + syncPos, remaining);
            buffered = remaining;
            continue;
        }

        if (!havePrevFrame || currFrame.frequency_knob != prevFrame.frequency_knob) { 
            std::cout << "Freqncy Knob Change " << currFrame.frequency_knob << '\n' ;
        }

        if (!havePrevFrame || currFrame.ptt_dac != prevFrame.ptt_dac) { 
            std::cout << "PTT DAC line change " << currFrame.ptt_dac << '\n' ;
        }

        if (!havePrevFrame || currFrame.volume_pot_dac != prevFrame.volume_pot_dac) { 
            std::cout << "Volume pot DAC hange " << currFrame.volume_pot_dac << '\n' ;
        }

        if (!havePrevFrame || currFrame.squelch_pot_dac != prevFrame.squelch_pot_dac) { 
            std::cout << "Squelch pot DAC hange " << currFrame.squelch_pot_dac << '\n' ;
        }


        if (!havePrevFrame ||
             currFrame.mic_sw1_dac != prevFrame.mic_sw1_dac || 
             currFrame.mic_sw2_dac != prevFrame.mic_sw2_dac 
           ) {
            std::cout << "MIC Keypad DAC lines change Row:" << currFrame.mic_sw1_dac << " Column: " << currFrame.mic_sw2_dac << '\n' ;
        }

        if (!havePrevFrame || currFrame.button1_dac != prevFrame.button1_dac) { 
            std::cout << "Button 2 DAC hange " << currFrame.button1_dac << '\n' ;
        }

        if (!havePrevFrame || currFrame.button2_dac != prevFrame.button2_dac) { 
            std::cout << "Button 2 DAC hange " << currFrame.button2_dac << '\n' ;
        }

        if (!havePrevFrame || currFrame.screen_buttons != prevFrame.screen_buttons) { 
            std::cout << "Screen buttons byte change " << currFrame.screen_buttons << '\n' ;
        }



        prevFrame = currFrame;
        havePrevFrame = true;
        buffered = 0;
    }
}


SerialConfig loadConfig(const std::string& configFile) {
    libconfig::Config cfg;
    cfg.readFile(configFile.c_str());

    SerialConfig config;
    const libconfig::Setting& root = cfg.getRoot();

    const libconfig::Setting& serial = root["serial"];
    serial.lookupValue("port", config.port);
    serial.lookupValue("baud_rate", config.baud_rate);
    serial.lookupValue("data_bits", config.data_bits);
    serial.lookupValue("parity", config.parity);
    serial.lookupValue("stop_bits", config.stop_bits);
    serial.lookupValue("send_interval_ms", config.send_interval_ms);

    return config;
}

void updateDisplay(DisplayState& displaystate) {
    auto nextTimePoint = std::chrono::steady_clock::now();

    displaystate.clear();

    /* Turn em on */
    for (unsigned int i = 0; i < ALL_DISPLAY_ELEMENTS_MAX; i++) {
        displaystate.setBit(ALL_DISPLAY_ELEMENTS[i]);
    }

    // Commit changes to the current buffer
    displaystate.setBacklightLevel(5);
    displaystate.commit();

    while (true) {
        displaystate.clear();
        displaystate.setBacklightLevel(5);

        /* Turn em all on */
        for (uint32_t i = 0; i < ALL_DISPLAY_ELEMENTS_MAX; i++) {
            displaystate.setBit(ALL_DISPLAY_ELEMENTS[i]);
        }

        for (uint32_t i = 0; i < ALL_DISPLAY_ELEMENTS_MAX; i++) {
            /* Skip high bit of serial byte */
            if ((i % 8) == 7) i++;

            std::cout << "testing byte:  " << i / 8 << "bit " << i % 8
                      << " count " << static_cast<uint32_t>(i % ALL_DISPLAY_ELEMENTS_MAX)
                      << " of " << ALL_DISPLAY_ELEMENTS_MAX << '\n';

            displaystate.setBacklightLevel(5);
            displaystate.setFrequencyDisplaySegment(0, (i / 100) % 10 + '0');
            displaystate.setFrequencyDisplaySegment(1, (i / 10) % 10 + '0');
            displaystate.setFrequencyDisplaySegment(2, i % 10 + '0');

            // displaystate.setMemoryChannelDisplaySegment(0, (i/100) % 10 + '0');
            // displaystate.setMemoryChannelDisplaySegment(1, (i/10) % 10 + '0');
            // displaystate.setMemoryChannelDisplaySegment(2, (i%10) + '0');

            /* Two flashes per element (5 toggles) */
            for (uint8_t repeat = 0; repeat < 6; repeat++) {
                if (repeat % 2) {
                    displaystate.setBit(DisplayBitMap::ANNUNCIATOR_INTERNET_CONNECTOR_FEATURE_ACTIVE);
                    displaystate.setBit(ALL_DISPLAY_ELEMENTS[i % ALL_DISPLAY_ELEMENTS_MAX]);
                } else {
                    displaystate.clearBit(DisplayBitMap::ANNUNCIATOR_INTERNET_CONNECTOR_FEATURE_ACTIVE);
                    displaystate.clearBit(ALL_DISPLAY_ELEMENTS[i % ALL_DISPLAY_ELEMENTS_MAX]);
                }

                displaystate.commit();
                nextTimePoint += std::chrono::milliseconds(100);
                std::this_thread::sleep_until(nextTimePoint);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::string configFile = argc > 1 ? argv[1] : "config.cfg";

    DisplayState displaystate;
    try {
        SerialConfig cfg = loadConfig(configFile);

        int serialPort = open(cfg.port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (serialPort == -1) {
            perror("Error opening serial port");
            return 1;
        }

        struct termios tty{};
        if (tcgetattr(serialPort, &tty) != 0) {
            perror("Error getting terminal attributes");
            close(serialPort);
            return 1;
        }

        cfsetospeed(&tty, B19200);
        cfsetispeed(&tty, B19200);

        // Control flags
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;                 // 8-bit chars
        tty.c_cflag |= (CLOCAL | CREAD);    // Local connections, enable RX
        tty.c_cflag &= ~(PARENB | PARODD);  // no parity
        tty.c_cflag &= ~CSTOPB;             // 1 stop bit
        tty.c_cflag &= ~CRTSCTS;            // No hardware flow control

        // Input flags
        tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK |
                         ISTRIP | INLCR | IGNCR | ICRNL | IXON);

        // Output flags
        tty.c_oflag &= ~OPOST;  // raw output

        // Local flags
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

        if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
            perror("Error setting serial port attributes");
            close(serialPort);
            return 1;
        }


        std::thread sender(sendData, serialPort, std::ref(displaystate), cfg.send_interval_ms);
        std::thread reader(readData, serialPort);
        std::thread update(updateDisplay, std::ref(displaystate));

        sender.join();
        reader.join();
        update.join();

        close(serialPort);
    } catch (const libconfig::FileIOException& ex) {
        std::cerr << "File I/O error: " << ex.what() << std::endl;
        return 1;
    } catch (const libconfig::ParseException& ex) {
        std::cerr << "Parse error at " << ex.getFile() << ":" << ex.getLine()
                  << " - " << ex.getError() << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

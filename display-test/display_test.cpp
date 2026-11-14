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

using FrameCallback = void (*)(int index, uint8_t value);
FrameCallback frameCallback = nullptr;

// The received input frame layout, in wire order.
struct InputFrame {
    uint8_t syncByte;
    uint8_t frequencyKnob;
    uint8_t pttDac;
    uint8_t byte3;
    uint8_t micSw1Dac;
    uint8_t micSw2Dac;
    uint8_t volumePotDac;
    uint8_t squelchPotDac;
    uint8_t duplicateMicSw2Dac;
    uint8_t button1Dac;
    uint8_t button2Dac;
    uint8_t button3Dac;
    uint8_t screenButtons;
};

static_assert(sizeof(InputFrame) == 13, "InputFrame must match the 13-byte wire frame");
static_assert(offsetof(InputFrame, syncByte) == 0);
static_assert(offsetof(InputFrame, frequencyKnob) == 1);
static_assert(offsetof(InputFrame, pttDac) == 2);
static_assert(offsetof(InputFrame, byte3) == 3);
static_assert(offsetof(InputFrame, micSw1Dac) == 4);
static_assert(offsetof(InputFrame, micSw2Dac) == 5);
static_assert(offsetof(InputFrame, volumePotDac) == 6);
static_assert(offsetof(InputFrame, squelchPotDac) == 7);
static_assert(offsetof(InputFrame, duplicateMicSw2Dac) == 8);
static_assert(offsetof(InputFrame, button1Dac) == 9);
static_assert(offsetof(InputFrame, button2Dac) == 10);
static_assert(offsetof(InputFrame, button3Dac) == 11);
static_assert(offsetof(InputFrame, screenButtons) == 12);

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

        // Compare by field name, while keeping the existing callback's wire
        // byte index for consumers that still use the generic callback.
        if (frameCallback) {
#define REPORT_CHANGED_FIELD(field) \
            do { \
                if (!havePrevFrame || currFrame.field != prevFrame.field) { \
                    frameCallback(static_cast<int>(offsetof(InputFrame, field)), currFrame.field); \
                } \
            } while (false)
            REPORT_CHANGED_FIELD(syncByte);
            REPORT_CHANGED_FIELD(frequencyKnob);
            REPORT_CHANGED_FIELD(pttDac);
            REPORT_CHANGED_FIELD(byte3);
            REPORT_CHANGED_FIELD(micSw1Dac);
            REPORT_CHANGED_FIELD(micSw2Dac);
            REPORT_CHANGED_FIELD(volumePotDac);
            REPORT_CHANGED_FIELD(squelchPotDac);
            REPORT_CHANGED_FIELD(duplicateMicSw2Dac);
            REPORT_CHANGED_FIELD(button1Dac);
            REPORT_CHANGED_FIELD(button2Dac);
            REPORT_CHANGED_FIELD(button3Dac);
            REPORT_CHANGED_FIELD(screenButtons);
#undef REPORT_CHANGED_FIELD
        }

        prevFrame = currFrame;
        havePrevFrame = true;
        buffered = 0;
    }
}

void registerFrameCallback(FrameCallback callback) {
    frameCallback = callback;
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

        registerFrameCallback([](int index, uint8_t value) {
            std::cout << "Byte changed at index " << index
                      << ": new value = 0x" << std::hex << +value << std::dec << '\n';
        });

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

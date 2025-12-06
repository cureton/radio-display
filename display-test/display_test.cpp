#include <iostream>
#include <vector>
#include <thread>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <libconfig.h++>
#include <cstring>


#include "../display-state/DisplayState.h"


using FrameCallback = void (*)(int index, uint8_t value);
FrameCallback frameCallback = nullptr;

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

//void sendData(int serialPort, const std::vector<uint8_t>& data, int interval_ms) {
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
    constexpr size_t FRAME_SIZE = 13;

    std::vector<uint8_t> prevFrame(FRAME_SIZE, 0x00);
    std::vector<uint8_t> currFrame(FRAME_SIZE, 0x00);

    while (true) {
	/* Read single bytes until we get sync with SOF byte */ 
        size_t bytesRead = read(serialPort, currFrame.data(), 1);

        if (bytesRead <= 0) continue;

        if (currFrame[0] == SYNC_BYTE) {
            /* First byte is start of frame in start of current frame */
            size_t bytesReadTotal = 1;

            while (bytesReadTotal < FRAME_SIZE) {
                size_t result = read(serialPort, currFrame.data() + bytesReadTotal, FRAME_SIZE - bytesReadTotal);
                if (result <= 0) {
                    perror("Error reading frame");
                    break;
                }
                bytesReadTotal += result;
            }

//            // Print the frame in hexadecimal format
//            std::cout << "Received Frame: ";
//            for (size_t i = 0; i < FRAME_SIZE; ++i) {
//                printf("%02X ", currFrame[i]);
//            }
//            std::cout << std::endl;
//
            // Compare frames and invoke callback if registered
            for (size_t i = 0; i < FRAME_SIZE; ++i) {
                if (currFrame[i] != prevFrame[i] && frameCallback) {
                    frameCallback(i, currFrame[i]);
                }
            }

            prevFrame = currFrame;
        }
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
    for (unsigned int i =0; i < ALL_DISPLAY_ELEMENTS_MAX; i++)
    {
        displaystate.setBit(ALL_DISPLAY_ELEMENTS[i]);
    }

   // Commit changes to the current buffer

    displaystate.setBacklightLevel(5);
    displaystate.commit();


    while (true) 
    {
        displaystate.clear();
        displaystate.setBacklightLevel(5);
	
	/* Turn em all on */
        for (uint32_t i = 0; i < ALL_DISPLAY_ELEMENTS_MAX; i++) 
        {
            displaystate.setBit(ALL_DISPLAY_ELEMENTS[i]);
        }

        for (uint32_t i = 0; i < ALL_DISPLAY_ELEMENTS_MAX; i++) 
        {
            /* Skip high bit of serial bite */	
            if (( i % 8) ==7 ) i++; 
    
            std::cout << "testing byte:  " << i / 8  << "bit " << i % 8  << " count " << (uint32_t) i % ALL_DISPLAY_ELEMENTS_MAX <<  " of "  <<  ALL_DISPLAY_ELEMENTS_MAX << '\n';
            DisplayBitMap bitindex = static_cast<DisplayBitMap>(i); 
    
            displaystate.setBacklightLevel(5);
            displaystate.setFrequencyDisplaySegment(0, (i/100) % 10   + '0');
            displaystate.setFrequencyDisplaySegment(1, (i/10)  % 10   + '0');
            displaystate.setFrequencyDisplaySegment(2, i       % 10   + '0');
    
	    //displaystate.setMemoryChannelDisplaySegment(0, (i/100) % 10   + '0');
            //displaystate.setMemoryChannelDisplaySegment(1, (i/10)  % 10   + '0');
            //displaystate.setMemoryChannelDisplaySegment(2, i       % 10   + '0');
    

            /* Two flashed per element (5 toggles) */
            for (uint8_t repeat=0; repeat < 6; repeat++) {
    
                if (repeat % 2)  {
                    displaystate.setBit(DisplayBitMap::ANNUNCIATOR_INTERNET_CONNECTOR_FEATURE_ACTIVE);
                    displaystate.setBit(ALL_DISPLAY_ELEMENTS[i % ALL_DISPLAY_ELEMENTS_MAX]);
                }  else {
                    displaystate.clearBit(DisplayBitMap::ANNUNCIATOR_INTERNET_CONNECTOR_FEATURE_ACTIVE);
                    displaystate.clearBit(ALL_DISPLAY_ELEMENTS[i % ALL_DISPLAY_ELEMENTS_MAX]);
                }
        
                displaystate.commit();
        
                // Schedule the next transmission
                nextTimePoint += std::chrono::milliseconds(100);
        
                // Wait until the next scheduled time
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
//
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
         tty.c_cflag |= CS8;                // 8-bit chars
         tty.c_cflag |= (CLOCAL | CREAD) ;  // Local connections, enable RX
         tty.c_cflag &= ~(PARENB | PARODD); // no parity
         tty.c_cflag &= ~CSTOPB;            // 1 stop bit
         tty.c_cflag &= ~CRTSCTS;            // No hardare flow control 

         // Input flags
         tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK |
                     ISTRIP | INLCR | IGNCR | ICRNL | IXON);

         // Output flags
         tty.c_oflag &= ~OPOST;              // raw output

         // Local flags
         tty.c_lflag &= ~(ICANON| ECHO | ECHOE| ISIG);


        if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
            perror("Error setting terminal attributes");
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

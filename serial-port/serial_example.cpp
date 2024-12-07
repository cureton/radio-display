#include <iostream>
#include <vector>
#include <thread>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <libconfig.h++>
#include <cstring>

using FrameCallback = void (*)(int index, uint8_t value);
FrameCallback frameCallback = nullptr;

struct SerialConfig {
    std::string port;
    int baud_rate;
    int data_bits;
    std::string parity;
    int stop_bits;
    int send_interval_ms;
    std::vector<uint8_t> data_bytes;
};

speed_t getBaudRate(int baud) {
    switch (baud) {
        case 19200: return B19200;
        case 9600:  return B9600;
        case 115200: return B115200;
        default: throw std::invalid_argument("Unsupported baud rate");
    }
}

void sendData(int serialPort, const std::vector<uint8_t>& data, int interval) {
    while (true) {
        ssize_t written = write(serialPort, data.data(), data.size());
        if (written == -1) {
            perror("Error writing to serial port");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
}

void readData(int serialPort) {
    constexpr uint8_t SYNC_BYTE = 0x80;
    constexpr size_t FRAME_SIZE = 13;

    std::vector<uint8_t> prevFrame(FRAME_SIZE, 0x00);
    std::vector<uint8_t> currFrame(FRAME_SIZE, 0x00);

    while (true) {
        uint8_t byte;
        ssize_t bytesRead = read(serialPort, &byte, 1);

        if (bytesRead <= 0) continue;

        if (byte == SYNC_BYTE) {
            ssize_t bytesReadTotal = 0;

            while (bytesReadTotal < FRAME_SIZE) {
                ssize_t result = read(serialPort, currFrame.data() + bytesReadTotal, FRAME_SIZE - bytesReadTotal);
                if (result <= 0) {
                    perror("Error reading frame");
                    break;
                }
                bytesReadTotal += result;
            }

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

    const libconfig::Setting& data = root["data"]["bytes"];
    for (int i = 0; i < data.getLength(); ++i) {
        config.data_bytes.push_back(static_cast<uint8_t>(data[i].operator int()));
    }

    return config;
}

int main(int argc, char* argv[]) {
    std::string configFile = argc > 1 ? argv[1] : "config.cfg";

    try {
        SerialConfig cfg = loadConfig(configFile);

        int serialPort = open(cfg.port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (serialPort == -1) {
            perror("Error opening serial port");
            return 1;
        }

        termios tty{};
        if (tcgetattr(serialPort, &tty) != 0) {
            perror("Error getting terminal attributes");
            close(serialPort);
            return 1;
        }

        cfsetospeed(&tty, getBaudRate(cfg.baud_rate));
        cfsetispeed(&tty, getBaudRate(cfg.baud_rate));

        tty.c_cflag = CLOCAL | CREAD | (cfg.data_bits == 8 ? CS8 : CS7);

        if (cfg.parity == "even") tty.c_cflag |= PARENB;
        else if (cfg.parity == "odd") tty.c_cflag |= (PARENB | PARODD);
        else tty.c_cflag &= ~PARENB;

        if (cfg.stop_bits == 2) tty.c_cflag |= CSTOPB;
        else tty.c_cflag &= ~CSTOPB;

        tty.c_iflag = IGNPAR;
        tty.c_oflag = 0;
        tty.c_lflag = 0;

        if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
            perror("Error setting terminal attributes");
            close(serialPort);
            return 1;
        }

        registerFrameCallback([](int index, uint8_t value) {
            std::cout << "Byte changed at index " << index 
                      << ": new value = 0x" << std::hex << +value << std::dec << '\n';
        });

        std::thread sender(sendData, serialPort, cfg.data_bytes, cfg.send_interval_ms);
        std::thread reader(readData, serialPort);

        sender.join();
        reader.join();

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

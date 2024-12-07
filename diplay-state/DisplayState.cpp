#include <vector> 
#include <iostream> 
#include <iomanip> 

#include "DisplayState.h"

// Constructor: Initialize the default state
DisplayState::DisplayState() {
    buffer_next[static_cast<size_t>(DisplayBitMap::UNKNOWN_0_BIT7)] = true;
}

// Set a specific bit
void DisplayState::setBit(DisplayBitMap bit) {
    buffer_next[static_cast<size_t>(bit)] = true;
}

// Clear a specific bit
void DisplayState::clearBit(DisplayBitMap bit) {
    buffer_next[static_cast<size_t>(bit)] = false;
}

// Commit changes from next buffer to current buffer
void DisplayState::commit() {
    buffer_current = buffer_next;
}

void DisplayState::printByteArray() const {
    size_t byteCount = (BITFIELD_SIZE + 7) / 8;  // Number of bytes required
    std::vector<uint8_t> byteArray(byteCount, 0);  // Use std::vector instead

    // Convert the sparse bitfield to the byte array
    for (size_t i = 0; i < BITFIELD_SIZE; ++i) {
        auto it = buffer_current.find(i);
        if (it != buffer_current.end() && it->second) {
            byteArray[i / 8] |= (1 << (i % 8));  // Set the bit in the correct byte
        }
    }

    // Print the byte array as hexadecimal
    for (const auto &byte : byteArray) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;  // Switch back to decimal for further output
}

std::vector<uint8_t> DisplayState::serialize() const {
    size_t byteCount = (BITFIELD_SIZE + 7) / 8;
    std::vector<uint8_t> byteArray(byteCount, 0);  // Initialize all bytes to 0

    // Populate the byte array from the sparse bitfield
    for (const auto& [bitIndex, isSet] : buffer_current) {
        if (isSet) {
            size_t byteIndex = bitIndex / 8;
            size_t bitPos = bitIndex % 8;
            byteArray[byteIndex] |= (1 << bitPos);
        }
    }
    return byteArray;
}

void DisplayState::deserialize(const std::vector<uint8_t>& byteArray) {
    buffer_current.clear();  // Reset the current buffer

    // Ensure the input size matches expected bytes
    size_t expectedBytes = (BITFIELD_SIZE + 7) / 8;
    if (byteArray.size() != expectedBytes) {
        std::cerr << "Error: Received byte array has incorrect size!" << std::endl;
        return;
    }

    // Convert byte array to the sparse bitfield
    for (size_t i = 0; i < BITFIELD_SIZE; ++i) {
        size_t byteIndex = i / 8;
        size_t bitIndex = i % 8;
        if (byteArray[byteIndex] & (1 << bitIndex)) {
            buffer_current[i] = true;
        }
    }
}


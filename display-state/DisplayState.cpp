
#include <vector> 
#include <iostream> 
#include <iomanip> 

#include "DisplayState.h"

#include "FontMap.h"


// Constructor: Initialize the default state
DisplayState::DisplayState() {
    buffer_next[static_cast<size_t>(DisplayBitMap::START_OF_FRAME_0_BIT7)] = true;
}


void DisplayState::clear() {
    buffer_next.clear();
    buffer_next[static_cast<size_t>(DisplayBitMap::START_OF_FRAME_0_BIT7)] = true;
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
    /* Ensure top bit of byte one is set - Start-of-fram */
    buffer_next[static_cast<size_t>(DisplayBitMap::START_OF_FRAME_0_BIT7)] = true;
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




void DisplayState::setBacklightLevel(uint8_t level) {

    if (level & 0x01)
	setBit(DisplayBitMap::BACKLIGHT_LEVEL_BIT_0);
    else 
        clearBit(DisplayBitMap::BACKLIGHT_LEVEL_BIT_0);

    if (level & 0x02)
	setBit(DisplayBitMap::BACKLIGHT_LEVEL_BIT_1);
    else 
        clearBit(DisplayBitMap::BACKLIGHT_LEVEL_BIT_1);

    if (level & 0x04)
	setBit(DisplayBitMap::BACKLIGHT_LEVEL_BIT_2);
    else 
        clearBit(DisplayBitMap::BACKLIGHT_LEVEL_BIT_2);

}

void DisplayState::setFrequencyDisplaySegment(uint8_t segment, char character) {

    constexpr uint8_t maxSegment = sizeof(OPERATING_FREQUENCY_DIGITS) / sizeof(OPERATING_FREQUENCY_DIGITS[0]);
    constexpr uint8_t maxSegmentElements = sizeof(OPERATING_FREQUENCY_DIGITS[0]) / sizeof(OPERATING_FREQUENCY_DIGITS[0][0]);

    if (segment >= maxSegment) { 
        std::cerr << "Segment index out of range" << std::endl;
	return; 
    }

    // Retrieve the 14-segment bit pattern for the character
    auto it = FOURTEEN_SEGMENT_FONT_MAP.find(character);
    if (it == FOURTEEN_SEGMENT_FONT_MAP.end()) {
        std::cerr << "Character not found in font map" << std::endl;
        return;
    }

    uint16_t fontBits = it->second;

    // Iterate over each bit position
    for (int i = 0; i < maxSegmentElements; ++i) {
        if (fontBits & (1 << i)) {
             DisplayState::setBit(OPERATING_FREQUENCY_DIGITS[segment][i]);
        } else {
             DisplayState::clearBit(OPERATING_FREQUENCY_DIGITS[segment][i]);
        }
    }
}

void DisplayState::setUnknownBit(uint8_t index) {

    constexpr uint8_t maxElements = sizeof(UNKNOWN_BITS) / sizeof(UNKNOWN_BITS[0]);

    std::cout << "bit intex " << (uint32_t)UNKNOWN_BITS[index] << " ";
    if (index < maxElements) { 
        DisplayState::setBit(UNKNOWN_BITS[index]);
         index ++;
    } else {
        std::cerr << "Unknown bit index out of range" << std::endl;
    }
    return; 
}

void DisplayState::clearUnknownBit(uint8_t index) {

    constexpr uint8_t maxElements = sizeof(UNKNOWN_BITS) / sizeof(UNKNOWN_BITS[0]);

    if (index < maxElements) { 
        DisplayState::clearBit(UNKNOWN_BITS[index]);
         index ++;
    } else {
        std::cerr << "Unknown bit index out of range" << std::endl;
    }
    return; 
}

void DisplayState::setMemoryChannelDisplaySegment(uint8_t segment, char character) {

    constexpr uint8_t maxSegment = sizeof(MEMORY_CHANNEL_DIGITS) / sizeof(MEMORY_CHANNEL_DIGITS[0]);
    constexpr uint8_t maxSegmentElements = sizeof(MEMORY_CHANNEL_DIGITS[0]) / sizeof(MEMORY_CHANNEL_DIGITS[0][0]);

    if (segment >= maxSegment) { 
        std::cerr << "Segment index out of range" << std::endl;
	return; 
    }

    // Retrieve the 14-segment bit pattern for the character
    auto it = FOURTEEN_SEGMENT_FONT_MAP.find(character);
    if (it == FOURTEEN_SEGMENT_FONT_MAP.end()) {
        std::cerr << "Character not found in font map" << std::endl;
        return;
    }

    uint16_t fontBits = it->second;

    // Iterate over each bit position
    for (int i = 0; i < maxSegmentElements; ++i) {
        if (fontBits & (1 << i)) {
             DisplayState::setBit(MEMORY_CHANNEL_DIGITS[segment][i]);
        } else {
             DisplayState::clearBit(MEMORY_CHANNEL_DIGITS[segment][i]);
        }
    }
}



void DisplayState::setPowerLevelIndicators(uint8_t level) {
    constexpr uint8_t maxLevel = S_AND_PO_LEVEL_MAX;

    // Limit level to the maximum allowed by the array
    level = (level > maxLevel) ? maxLevel : level;

    // Cylce throught Signal and Power Output Inticators and set them accorging to level 
    for (uint8_t i = 0; i < maxLevel; ++i) {
        if (i < level) {
             DisplayState::setBit(S_AND_PO_LEVEL[i]);
        } else {
             DisplayState::clearBit(S_AND_PO_LEVEL[i]);
        }
    }
}


void DisplayState::setAnnunciators(uint8_t level) {
    constexpr uint8_t maxElement = ANNUNCIATOR_BITS_MAX;

    // Limit level to the maximum allowed by the array
    level = (level > maxElement) ? maxElement : level;

    // Cylce throught Signal and Power Output Inticators and set them accorging to level 
    for (uint8_t i = 0; i < maxElement; ++i) {
        if (i < level) {
             DisplayState::setBit(ANNUNCIATOR_BITS[i]);
        } else {
             DisplayState::clearBit(ANNUNCIATOR_BITS[i]);
        }
    }
}

#include "DisplayState.h"

#include <algorithm>
#include <iostream>
#include <iomanip>

#include "FontMap.h"

namespace {
constexpr size_t bitIndex(DisplayBitMap bit) {
    return static_cast<size_t>(bit);
}
}

// Constructor: initialize the default state with the start-of-frame bit set.
DisplayState::DisplayState() {
    setBit(DisplayBitMap::START_OF_FRAME_0_BIT7);
    commit();
}

void DisplayState::clear() {
    buffer_next.fill(0);
    setBit(DisplayBitMap::START_OF_FRAME_0_BIT7);
}

void DisplayState::setBit(DisplayBitMap bit) {
    const size_t index = bitIndex(bit);
    if (bit == DisplayBitMap::UNUSED || index >= BITFIELD_SIZE) {
        return;
    }

    buffer_next[index / 8] |= static_cast<uint8_t>(1u << (index % 8));
}

void DisplayState::clearBit(DisplayBitMap bit) {
    const size_t index = bitIndex(bit);
    if (bit == DisplayBitMap::UNUSED || index >= BITFIELD_SIZE) {
        return;
    }

    buffer_next[index / 8] &= static_cast<uint8_t>(~(1u << (index % 8)));
}

void DisplayState::commit() {
    // The top bit of the first byte marks the start of a frame.
    setBit(DisplayBitMap::START_OF_FRAME_0_BIT7);
    buffer_current = buffer_next;
}

void DisplayState::printByteArray() const {
    for (const uint8_t byte : buffer_current) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(byte) << ' ';
    }
    std::cout << std::dec << std::endl;
}

std::vector<uint8_t> DisplayState::serialize() const {
    // The display state is already stored in wire-byte order; copy it directly.
    return std::vector<uint8_t>(buffer_current.begin(), buffer_current.end());
}

void DisplayState::deserialize(const std::vector<uint8_t>& byteArray) {
    if (byteArray.size() != BYTE_COUNT) {
        std::cerr << "Error: Received byte array has incorrect size! Expected "
                  << BYTE_COUNT << " bytes, got " << byteArray.size() << "."
                  << std::endl;
        return;
    }

    // Copy received wire bytes directly into the state buffers.
    std::copy(byteArray.begin(), byteArray.end(), buffer_current.begin());
    buffer_next = buffer_current;
}

void DisplayState::setBacklightLevel(uint8_t level) {
    for (size_t i = 0; i < BACKLIGHT_LEVEL_BITS_MAX; ++i) {
        if (level & (1u << i)) {
            setBit(BACKLIGHT_LEVEL_BITS[i]);
        } else {
            clearBit(BACKLIGHT_LEVEL_BITS[i]);
        }
    }
}

void DisplayState::setFrequencyDisplaySegment(uint8_t segment, char character) {
    constexpr size_t maxSegments =
        sizeof(OPERATING_FREQUENCY_DIGITS) / sizeof(OPERATING_FREQUENCY_DIGITS[0]);
    constexpr size_t segmentElements =
        sizeof(OPERATING_FREQUENCY_DIGITS[0]) / sizeof(OPERATING_FREQUENCY_DIGITS[0][0]);

    if (segment >= maxSegments) {
        std::cerr << "Segment index out of range" << std::endl;
        return;
    }

    uint16_t fontBits = 0;
    if (!lookupFont14(character, fontBits)) {
        std::cerr << "Character not found in font map" << std::endl;
        return;
    }

    for (size_t i = 0; i < segmentElements; ++i) {
        if (fontBits & (1u << i)) {
            setBit(OPERATING_FREQUENCY_DIGITS[segment][i]);
        } else {
            clearBit(OPERATING_FREQUENCY_DIGITS[segment][i]);
        }
    }
}

void DisplayState::setUnknownBit(uint8_t index) {
    if (index >= UNKNOWN_BITS_MAX) {
        std::cerr << "Unknown bit index out of range" << std::endl;
        return;
    }
    setBit(UNKNOWN_BITS[index]);
}

void DisplayState::clearUnknownBit(uint8_t index) {
    if (index >= UNKNOWN_BITS_MAX) {
        std::cerr << "Unknown bit index out of range" << std::endl;
        return;
    }
    clearBit(UNKNOWN_BITS[index]);
}

void DisplayState::setMemoryChannelDisplaySegment(uint8_t segment, char character) {
    constexpr size_t maxSegments =
        sizeof(MEMORY_CHANNEL_DIGITS) / sizeof(MEMORY_CHANNEL_DIGITS[0]);
    constexpr size_t segmentElements =
        sizeof(MEMORY_CHANNEL_DIGITS[0]) / sizeof(MEMORY_CHANNEL_DIGITS[0][0]);

    if (segment >= maxSegments) {
        std::cerr << "Segment index out of range" << std::endl;
        return;
    }

    uint8_t fontBits = 0;
    if (!lookupFont8(character, fontBits)) {
        std::cerr << "Character not found in font map" << std::endl;
        return;
    }

    for (size_t i = 0; i < segmentElements; ++i) {
        if (fontBits & (1u << i)) {
            setBit(MEMORY_CHANNEL_DIGITS[segment][i]);
        } else {
            clearBit(MEMORY_CHANNEL_DIGITS[segment][i]);
        }
    }
}

void DisplayState::setPowerLevelIndicators(uint8_t level) {
    const size_t count = S_AND_PO_LEVEL_MAX;
    if (level > count) {
        level = static_cast<uint8_t>(count);
    }

    for (size_t i = 0; i < count; ++i) {
        if (i < level) {
            setBit(S_AND_PO_LEVEL[i]);
        } else {
            clearBit(S_AND_PO_LEVEL[i]);
        }
    }
}

void DisplayState::setAnnunciators(uint8_t level) {
    const size_t count = ANNUNCIATOR_BITS_MAX;
    if (level > count) {
        level = static_cast<uint8_t>(count);
    }

    for (size_t i = 0; i < count; ++i) {
        if (i < level) {
            setBit(ANNUNCIATOR_BITS[i]);
        } else {
            clearBit(ANNUNCIATOR_BITS[i]);
        }
    }
}

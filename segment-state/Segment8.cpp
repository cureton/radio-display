#include "Segment8.h"

Segment8::Segment8() : segments{} {}

void Segment8::setSegment(Segment segment, bool state) {
    uint8_t index = static_cast<uint8_t>(segment);
    auto* raw = reinterpret_cast<unsigned*>(&segments);
    if (state) {
        *raw |= (1U << index);
    } else {
        *raw &= ~(1U << index);
    }
}

bool Segment8::getSegment(Segment segment) const {
    uint8_t index = static_cast<uint8_t>(segment);
    const auto* raw = reinterpret_cast<const unsigned*>(&segments);
    return (*raw & (1U << index)) != 0;
}

void Segment8::setAllSegments(uint8_t bitfield) {
    segments = *reinterpret_cast<const SegmentBits*>(&bitfield);
}

void Segment8::setCharacter(char c) {
    if (c >= 0 && c < 128) {
        setAllSegments(characterBitfields[static_cast<int>(c)]);
    }
}

void Segment8::printState() const {
    const auto* raw = reinterpret_cast<const unsigned*>(&segments);
    std::bitset<8> state(*raw);
    std::cout << "Segment state: " << state << '\n';
}

// Initialize the bitfield array outside the class definition
uint8_t Segment8::characterBitfields[128] = {
    0b11111100, // 'A'
    0b11111010, // 'B'
    0b10111100, // 'C'
    0b11111000, // 'D'
    0b10111110, // 'E'
    // Add more characters as needed...
};

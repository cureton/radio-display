#include "Segment14.h"

Segment14::Segment14() : segments{} {}

void Segment14::setSegment(Segment segment, bool state) {
    uint8_t index = static_cast<uint8_t>(segment);
    auto* raw = reinterpret_cast<unsigned*>(&segments);
    if (state) {
        *raw |= (1U << index);
    } else {
        *raw &= ~(1U << index);
    }
}

bool Segment14::getSegment(Segment segment) const {
    uint8_t index = static_cast<uint8_t>(segment);
    const auto* raw = reinterpret_cast<const unsigned*>(&segments);
    return (*raw & (1U << index)) != 0;
}

void Segment14::setAllSegments(uint16_t bitfield) {
    segments = *reinterpret_cast<const SegmentBits*>(&bitfield);
}

void Segment14::setCharacter(char c) {
    if (c >= 0 && c < 128) {
        setAllSegments(characterBitfields[static_cast<int>(c)]);
    }
}

void Segment14::printState() const {
    const auto* raw = reinterpret_cast<const unsigned*>(&segments);
    std::bitset<14> state(*raw);
    std::cout << "Segment state: " << state << '\n';
}

// Initialize the bitfield array outside the class definition
uint16_t Segment14::characterBitfields[128] = {
    0b1111110000000000, // 'A'
    0b1111101000000000, // 'B'
    0b1011110000000000, // 'C'
    0b1111100000000000, // 'D'
    0b1011111000000000, // 'E'
    // Add more characters as needed...
};

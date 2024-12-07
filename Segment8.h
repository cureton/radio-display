#ifndef SEGMENT8_H
#define SEGMENT8_H

#include <bitset>
#include <iostream>
#include <cstdint>

class Segment8 {
public:
    enum Segment {
        A, B, C, D, E, F, G, H
    };

    Segment8();
    void setSegment(Segment segment, bool state);
    bool getSegment(Segment segment) const;
    void setAllSegments(uint8_t bitfield);
    void setCharacter(char c);
    void printState() const;

    // Declare the static array, without initializing it here
    static uint8_t characterBitfields[128];

private:
    struct SegmentBits {
        unsigned A : 1;
        unsigned B : 1;
        unsigned C : 1;
        unsigned D : 1;
        unsigned E : 1;
        unsigned F : 1;
        unsigned G : 1;
        unsigned H : 1;
    } segments;
};

#endif // SEGMENT8_H

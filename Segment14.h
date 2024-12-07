#ifndef SEGMENT14_H
#define SEGMENT14_H

#include <bitset>
#include <iostream>
#include <cstdint>

class Segment14 {
public:
    enum Segment {
        A, B, C, D, E, F, G, H, I, J, K, L, M, N
    };

    Segment14();
    void setSegment(Segment segment, bool state);
    bool getSegment(Segment segment) const;
    void setAllSegments(uint16_t bitfield);
    void setCharacter(char c);
    void printState() const;

    // Declare the static array without initializing it here
    static uint16_t characterBitfields[128];

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
        unsigned I : 1;
        unsigned J : 1;
        unsigned K : 1;
        unsigned L : 1;
        unsigned M : 1;
        unsigned N : 1;
    } segments;
};

#endif // SEGMENT14_H

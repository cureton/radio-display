#include <iostream>
#include "Segment8.h"
#include "Segment14.h"

int main() {
    Segment8 display8;
    Segment14 display14;

    // Set individual segments for Segment8
    std::cout << "Setting individual segments for Segment8..." << std::endl;
    display8.setSegment(Segment8::A, true);
    display8.setSegment(Segment8::C, true);
    display8.setSegment(Segment8::G, true);
    display8.printState();

    // Set the entire display for Segment8 using a character
    std::cout << "\nSetting Segment8 to character 'A'..." << std::endl;
    display8.setCharacter('A' - 'A');
    display8.printState();

    // Set individual segments for Segment14
    std::cout << "\nSetting individual segments for Segment14..." << std::endl;
    display14.setSegment(Segment14::A, true);
    display14.setSegment(Segment14::C, true);
    display14.setSegment(Segment14::G, true);
    display14.printState();

    // Set the entire display for Segment14 using a character
    std::cout << "\nSetting Segment14 to character 'A'..." << std::endl;
    display14.setCharacter('A' - 'A');
    display14.printState();

    // Set Segment8 and Segment14 to character 'B'
    std::cout << "\nSetting Segment8 and Segment14 to character 'B'..." << std::endl;
    display8.setCharacter('B' - 'A');
    display8.printState();

    display14.setCharacter('B' - 'A');
    display14.printState();

    return 0;
}

#include "DisplayState.h"

int main() {
    DisplayState displaystate;

    // Set some bits using the enum
    displaystate.setBit(DisplayBitMap::ANNUNCIATOR_9600BPS);
    displaystate.setBit(DisplayBitMap::ANNUNCIATOR_LOW_TX_POWER);
    displaystate.setBit(DisplayBitMap::UNKNOWN_36_BIT4);
    displaystate.setBit(DisplayBitMap::MEMORY_CHANNEL_SEPARATOR);
    displaystate.setBit(DisplayBitMap::ANNUNCIATOR_INTERNET_CONNECTOR_FEATURE_ACTIVE);

    // Commit changes to the current buffer
    displaystate.commit();

    // Print the byte array
    displaystate.printByteArray();

    return 0;
}

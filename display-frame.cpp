#include <iostream>
#include <unordered_map>
#include <iomanip>

// Enum to label each bit in a type-safe way
enum class DisplayBitMap : size_t {
    ANNUNCIATOR_9600BPS = 0,
    OPERATING_FREQ_DIGIT_1_SEG_E,
    OPERATING_FREQ_DIGIT_1_SEG_L,
    OPERATING_FREQ_DIGIT_1_SEG_G1,
    OPERATING_FREQ_DIGIT_1_SEG_H,
    OPERATING_FREQ_DIGIT_1_SEG_F,
    OPERATING_FREQ_DIGIT_1_SEG_A,
    UNKNOWN_0_BIT7,

    ANNUNCIATOR_PREFERENTIAL_MEMORY_CHANNEL = 8,
    OPERATING_FREQ_DIGIT_1_SEG_K,
    OPERATING_FREQ_DIGIT_1_SEG_J,
    OPERATING_FREQ_DIGIT_1_SEG_B,
    OPERATING_FREQ_DIGIT_1_SEG_G2,
    OPERATING_FREQ_DIGIT_1_SEG_C,
    OPERATING_FREQ_DIGIT_1_SEG_N,
    UNKNOWN_1_BIT7,

    OPERATING_FREQ_DIGIT_1_SEG_D =16,
    ANNUNCIATOR_LOW_TX_POWER,
    ANNUNCIATOR_KEYPAD_LOCK_ACTIVE,
    OPERATING_FREQ_DIGIT_2_SEG_E,
    OPERATING_FREQ_DIGIT_2_SEG_L,
    OPERATING_FREQ_DIGIT_2_SEG_G1,
    OPERATING_FREQ_DIGIT_2_SEG_H,
    UNKNOWN_2_BIT7,

    OPERATING_FREQ_DIGIT_2_SEG_F = 24, 
    OPERATING_FREQ_DIGIT_2_SEG_A, 
    ANNUNCIATOR_SKIP_MEMORY_CHANNEL,
    OPERATING_FREQ_DIGIT_2_SEG_K, 
    OPERATING_FREQ_DIGIT_2_SEG_J, 
    OPERATING_FREQ_DIGIT_2_SEG_B, 
    OPERATING_FREQ_DIGIT_2_SEG_G2, 
    UNKNOWN_3_BIT7,

    OPERATING_FREQ_DIGIT_2_SEG_C = 32,
    OPERATING_FREQ_DIGIT_2_SEG_N,
    OPERATING_FREQ_DIGIT_2_SEG_D,
    ANNUNCIATOR_MEDIUM_TX_POWER,
    ANNUNCIATOR_BUSY_CHANNEL,
    OPERATING_FREQ_DIGIT_3_SEG_E,
    OPERATING_FREQ_DIGIT_3_SEG_L,

    OPERATING_FREQ_DIGIT_3_SEG_G1 = 40,
    OPERATING_FREQ_DIGIT_3_SEG_H,
    OPERATING_FREQ_DIGIT_3_SEG_F,
    OPERATING_FREQ_DIGIT_3_SEG_A,
    ANNUNCIATOR_DCS_OPERATION,
    OPERATING_FREQ_DIGIT_3_SEG_K,
    OPERATING_FREQ_DIGIT_3_SEG_J,
    UNKNOWN_5_BIT7,

    OPERATING_FREQ_DIGIT_3_SEG_B = 48,
    OPERATING_FREQ_DIGIT_3_SEG_G2,
    OPERATING_FREQ_DIGIT_3_SEG_C,
    OPERATING_FREQ_DIGIT_3_SEG_N,
    OPERATING_FREQ_DIGIT_3_SEG_D,
    ANNUNCIATOR_S_AND_PO_1,
    ANNUNCIATOR_S_AND_PO_2,
    UNKNOWN_6_BIT7,

    OPERATING_FREQ_DIGIT_3_SEG_DP = 56,
    ANNUNCIATOR_S_AND_PO_3,
    OPERATING_FREQ_DIGIT_4_SEG_E,
    UNKNOWN_7_BIT3,
    UNKNOWN_7_BIT4,
    UNKNOWN_7_BIT5,
    UNKNOWN_7_BIT6,
    UNKNOWN_7_BIT7,

    OPERATING_FREQ_DIGIT_4_SEG_L = 64,
    OPERATING_FREQ_DIGIT_4_SEG_G1,
    OPERATING_FREQ_DIGIT_4_SEG_H,
    OPERATING_FREQ_DIGIT_4_SEG_F,
    OPERATING_FREQ_DIGIT_4_SEG_A,
    ANNUNCIATOR_REPEATER_SHIFT_DIRECTION_MINUS,
    OPERATING_FREQ_DIGIT_4_SEG_K,
    UNKNOWN_8_BIT7,

    OPERATING_FREQ_DIGIT_4_SEG_J = 72,
    OPERATING_FREQ_DIGIT_4_SEG_B,
    OPERATING_FREQ_DIGIT_4_SEG_G2,
    OPERATING_FREQ_DIGIT_4_SEG_C,
    OPERATING_FREQ_DIGIT_4_SEG_N,
    OPERATING_FREQ_DIGIT_4_SEG_D,
    ANNUNCIATOR_REPEATER_SHIFT_DIRECTION_PLUS,
    UNKNOWN_9_BIT7,

    ANNUNCIATOR_S_AND_PO_4 = 80,
    OPERATING_FREQ_DIGIT_5_SEG_E,
    OPERATING_FREQ_DIGIT_5_SEG_L,
    OPERATING_FREQ_DIGIT_5_SEG_G1,
    OPERATING_FREQ_DIGIT_5_SEG_H,
    OPERATING_FREQ_DIGIT_5_SEG_F,
    OPERATING_FREQ_DIGIT_5_SEG_A,
    UNKNOWN_10_BIT7,

    ANNUNCIATOR_CTCSS_OPERATION_ENCODE = 88,
    OPERATING_FREQ_DIGIT_5_SEG_K,
    OPERATING_FREQ_DIGIT_5_SEG_J,
    OPERATING_FREQ_DIGIT_5_SEG_B,
    OPERATING_FREQ_DIGIT_5_SEG_G2,
    OPERATING_FREQ_DIGIT_5_SEG_C,
    OPERATING_FREQ_DIGIT_5_SEG_N,
    UNKNOWN_11_BIT7,

    OPERATING_FREQ_DIGIT_5_SEG_D = 96,
    ANNUNCIATOR_S_AND_PO_6,
    UNKNOWN_12_BIT2,
    OPERATING_FREQ_DIGIT_6_SEG_E,
    OPERATING_FREQ_DIGIT_6_SEG_L,
    OPERATING_FREQ_DIGIT_6_SEG_G1,
    OPERATING_FREQ_DIGIT_6_SEG_H,
    UNKNOWN_12_BIT7,

    OPERATING_FREQ_DIGIT_6_SEG_F = 104,
    OPERATING_FREQ_DIGIT_6_SEG_A,
    ANNUNCIATOR_CTCSS_OPERATION_DECODE,
    OPERATING_FREQ_DIGIT_6_SEG_K,
    OPERATING_FREQ_DIGIT_6_SEG_J,
    UNKNOWN_13_BIT5,
    OPERATING_FREQ_DIGIT_6_SEG_G2,
    UNKNOWN_13_BIT7,
   
    OPERATING_FREQ_DIGIT_6_SEG_C = 112,
    OPERATING_FREQ_DIGIT_6_SEG_N,
    OPERATING_FREQ_DIGIT_6_SEG_D,
    ANNUNCIATOR_S_AND_PO_7,
    OPERATING_FREQ_DIGIT_6_SEG_DP,
    ANNUNCIATOR_TRANSMISSION_IN_PROGRESS,
    ANNUNCIATOR_S_AND_PO_8,
    UNKNOWN_14_BIT7,

    ANNUNCIATOR_PLUS_500_HERTZ = 124, 
    ANNUNCIATOR_S_AND_PO_9,
    ANNUNCIATOR_AUTOMATIC_POWER_OFF_ACTIVE,
    UNKNOWN_15_BIT3,
    UNKNOWN_15_BIT4,
    UNKNOWN_15_BIT5,
    UNKNOWN_15_BIT6,
    UNKNOWN_15_BIT7,

    UNKNOWN_36_BIT0 = 288, 
    UNKNOWN_36_BIT1,
    UNKNOWN_36_BIT2,
    UNKNOWN_36_BIT3,
    UNKNOWN_36_BIT4,
    MEMORY_CHANNEL_DIGIT_1_SEG_B,
    MEMORY_CHANNEL_DIGIT_1_SEG_C,
    MEMORY_CHANNEL_DIGIT_1_SEG_A,

    MEMORY_CHANNEL_DIGIT_1_SEG_G = 296,
    MEMORY_CHANNEL_DIGIT_1_SEG_D,
    MEMORY_CHANNEL_DIGIT_1_SEG_E,
    ANNUNCIATOR_PRIORITORY_CHANNEL,
    MEMORY_CHANNEL_DIGIT_1_SEG_F,
    MEMORY_CHANNEL_DIGIT_2_SEG_B,
    MEMORY_CHANNEL_DIGIT_2_SEG_C,
    UNKNOWN_37_BIT7,

    MEMORY_CHANNEL_DIGIT_2_SEG_A = 304,
    MEMORY_CHANNEL_DIGIT_2_SEG_G,
    MEMORY_CHANNEL_DIGIT_2_SEG_D,
    MEMORY_CHANNEL_DIGIT_2_SEG_E,
    UNKNOWN_38_BIT4,
    MEMORY_CHANNEL_DIGIT_2_SEG_F,
    UNKNOWN_38_BIT6,
    UNKNOWN_38_BIT7,

    UNKNOWN_39_BIT0 = 312,
    UNKNOWN_39_BIT1,
    UNKNOWN_39_BIT2,
    MEMORY_CHANNEL_DIGIT_3_SEG_B,
    MEMORY_CHANNEL_DIGIT_3_SEG_C,
    MEMORY_CHANNEL_DIGIT_3_SEG_A,
    MEMORY_CHANNEL_DIGIT_3_SEG_G,
    UNKNOWN_39_BIT7,

    MEMORY_CHANNEL_DIGIT_3_SEG_D = 320,
    MEMORY_CHANNEL_DIGIT_3_SEG_E,
    ANNUNCIATOR_MEMORY_TUNE_MODE,
    MEMORY_CHANNEL_DIGIT_3_SEG_F,
    MEMORY_CHANNEL_SEPARATOR,
    ANNUNCIATOR_INTERNET_CONNECTOR_FEATURE_ACTIVE,
    ANNUNCIATOR_AMP_MODE_SELECTED,
    UNKNOWN_40_BIT7,

    ANNUNCIATOR_SET_MODE_ACTIVE = 328,
    BACKLIGHT_LEVEL_BIT_0,
    BACKLIGHT_LEVEL_BIT_1,
    BACKLIGHT_LEVEL_BIT_2,
    UNKNOWN_41_BIT4,
    UNKNOWN_41_BIT5,
    UNKNOWN_41_BIT6,
    UNKNOWN_41_BIT7
};

class DisplayState {
public:
    DisplayState() {
	// Set this bit high, as it always is
        buffer_next[static_cast<size_t>(DisplayBitMap::UNKNOWN_0_BIT7)] = true;
    }

    // Set a specific bit using the enum class
    void setBit(DisplayBitMap bit) {
        buffer_next[static_cast<size_t>(bit)] = true;
    }

    // Clear a specific bit using the enum class
    void clearBit(DisplayBitMap bit) {
        buffer_next[static_cast<size_t>(bit)] = false;
    }

    // Commit changes from the next buffer to the current buffer
    void commit() {
        buffer_current = buffer_next;
    }

    // Print the current buffer as a byte array suitable for serial transmission
    void printByteArray() const {
        size_t byteCount = (BITFIELD_SIZE + 7) / 8;  // Number of bytes required
        uint8_t byteArray[byteCount] = {0};

        // Convert the sparse bitfield to the byte array
        for (size_t i = 0; i < BITFIELD_SIZE; ++i) {
            auto it = buffer_current.find(i);
            if (it != buffer_current.end() && it->second) {
                byteArray[i / 8] |= (1 << (i % 8));  // Set the bit in the correct byte
            }
        }

        // Print the byte array as hexadecimal
        for (size_t i = 0; i < byteCount; ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byteArray[i] << " ";
        }
        std::cout << std::dec << std::endl;  // Switch back to decimal for further output
    }

private:
    std::unordered_map<size_t, bool> buffer_current;  // Sparse representation using size_t
    std::unordered_map<size_t, bool> buffer_next;     // Next buffer (for double buffering)
    static constexpr size_t BITFIELD_SIZE = 336;  // Total size of the bitfield (336 bits)
};

int main() {
    DisplayState displaystate;

    // Set some bits using the enum
    displaystate.setBit(DisplayBitMap::ANNUNCIATOR_9600BPS);   // Set BIT_0
    displaystate.setBit(DisplayBitMap::ANNUNCIATOR_LOW_TX_POWER);   // Set BIT_3
    displaystate.setBit(DisplayBitMap::UNKNOWN_36_BIT4);   // Set BIT_8 (9th bit)
    displaystate.setBit(DisplayBitMap::MEMORY_CHANNEL_SEPARATOR);   // Set BIT_8 (9th bit)
    displaystate.setBit(DisplayBitMap::ANNUNCIATOR_INTERNET_CONNECTOR_FEATURE_ACTIVE);   // Set BIT_8 (9th bit)

    // Commit changes to the current buffer
    displaystate.commit();

    // Print the byte array
    displaystate.printByteArray();

    return 0;
}

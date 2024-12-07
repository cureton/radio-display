#ifndef DISPLAYSTATE_H
#define DISPLAYSTATE_H

#include <iostream>
#include <unordered_map>
#include <iomanip>
#include <vector>

// Enum for display bit mapping
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
    DisplayState();  // Constructor

    // Public methods
    void setBit(DisplayBitMap bit);
    void clearBit(DisplayBitMap bit);
    void commit();
    void printByteArray() const;
    std::vector<uint8_t> serialize() const;
    void deserialize(const std::vector<uint8_t>& byteArray);


private:
    // Data members
    std::unordered_map<size_t, bool> buffer_current;
    std::unordered_map<size_t, bool> buffer_next;
    static constexpr size_t BITFIELD_SIZE = 336;
};

#endif  // DISPLAYSTATE_H
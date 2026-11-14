#ifndef KEYPADDECODER_H
#define KEYPADDECODER_H

#include <array>
#include <cstdint>
#include <functional>

enum class FrameOffset : uint8_t {
    SYNC_BYTE = 0,
    FREQUENCY_KNOB = 1,
    PTT_DAC = 2,
    BYTE3 = 3,
    MIC_SW1_DAC = 4,
    MIC_SW2_DAC = 5,
    VOLUME_POT_DAC = 6,
    SQUELCH_POT_DAC = 7,
    DUPICATE_MIC_SW2_DAC = 8,
    BUTTON1_DAC = 9,
    BUTTON2_DAC = 10,
    BUTTON3_DAC = 11,
    SCREEN_BUTTONS = 12,
};

struct InputFrame {
    uint8_t sync_byte;
    uint8_t frequency_knob;
    uint8_t ptt_dac;
    uint8_t byte3;
    uint8_t mic_sw1_dac;
    uint8_t mic_sw2_dac;
    uint8_t volume_pot_dac;
    uint8_t squelch_pot_dac;
    uint8_t duplicate_mic_sw2_dac;
    uint8_t button1_dac;
    uint8_t button2_dac;
    uint8_t button3_dac;
    uint8_t screen_buttons;
};

/* Assertions to catch alignment issues  */
static_assert(sizeof(InputFrame) == 13);
static_assert(offsetof(InputFrame, ptt_dac) == 2);
static_assert(offsetof(InputFrame, screen_buttons) == 12);
static_assert(std::is_trivially_copyable_v<InputFrame>);

enum class Key {
    DISPLAY_BTN_1,   // Byte 12 direct encoding, value 1
    DISPLAY_BTN_2,   // Byte 12 direct encoding, value 2
    DISPLAY_BTN_3,   // Byte 12 direct encoding, value 3
    DISPLAY_BTN_SEL, // Byte 12 direct encoding, value 3
    DISPLAY_BTN_4,   // Byte 12 direct encoding, value 5
    DISPLAY_BTN_5,   // Byte 12 direct encoding, value 6
    DISPLAY_BTN_MHZ_PRI,     // Byte 11 voltage encoding bit 2
    DISPLAY_BTN_TONE_HM_RV,  // Byte 11 voltage encoding bit 3
    DISPLAY_BTN_LOW_ACC,     // Byte 11 voltage encoding bit 1 
    DISPLAY_BTN_BAND_SET,    // Byte 10 voltage encoding
    DISPLAY_BTN_VM_MW,       // Byte 10 voltage encoding
    DISPLAY_BTN_SCAN_SEL,    // Byte 9 voltage encoding
    DISPLAY_BTN_SSCH_ARTS,   // Byte 9 voltage encoding

    MICROPHONE_BTN_1,
    MICROPHONE_BTN_2,
    MICROPHONE_BTN_3,
    MICROPHONE_BTN_4,
    MICROPHONE_BTN_5,
    MICROPHONE_BTN_6,
    MICROPHONE_BTN_7,
    MICROPHONE_BTN_8,
    MICROPHONE_BTN_9,
    MICROPHONE_BTN_0,
    MICROPHONE_BTN_STAR,
    MICROPHONE_BTN_HASH,
    MICROPHONE_BTN_A,
    MICROPHONE_BTN_B,
    MICROPHONE_BTN_C,
    MICROPHONE_BTN_D,
    MICROPHONE_BTN_PTT,
    MICROPHONE_BTN_P1,
    MICROPHONE_BTN_P2,
    MICROPHONE_BTN_P3,
    MICROPHONE_BTN_P4,
    MICROPHONE_BTN_DWN,
    MICROPHONE_BTN_UP,
    MICROPHONE_PTT, // Byte 2 direct encoding
};

enum class KeyAction {
    Pressed,
    Released,
};

struct KeyEvent {
    Key key;
    KeyAction action;
};

using KeyEventHandler = std::function<void(const KeyEvent&)>;

class KeypadDecoder {
public:
    explicit KeypadDecoder(KeyEventHandler handler);
    void processFrame(const std::array<uint8_t, 13>& frame);

private:
    KeyEventHandler handler_;
    std::array<uint8_t, 13> previousFrame_{};
    bool havePreviousFrame_ = false;
};

#endif  // KEYPADDECODER_H

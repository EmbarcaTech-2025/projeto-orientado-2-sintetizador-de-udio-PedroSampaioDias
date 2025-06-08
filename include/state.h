#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/mutex.h"
#include "config.h"

#define STATUS_LINE_SIZE 32
#define WAVEFORM_DISPLAY_SIZE 128

#define DEBOUNCE_DELAY_MS 200

typedef enum {
    MENU_SCREEN,
    RECORDING_SCREEN,
    WAVEFORM_SCREEN
} screen_mode_t;

extern uint16_t       audio_buffer[BUFFER_SIZE];
extern volatile bool  recording;
extern volatile bool  playing;
extern volatile bool  ready;
extern volatile bool  buffer_has_audio;

extern volatile uint32_t sample_index;
extern volatile screen_mode_t current_screen;

extern mutex_t        display_mutex;
extern char           status_line[STATUS_LINE_SIZE];

extern uint16_t waveform_display[WAVEFORM_DISPLAY_SIZE];
extern volatile bool waveform_ready;

extern volatile bool initial_screen_active;

#endif // STATE_H
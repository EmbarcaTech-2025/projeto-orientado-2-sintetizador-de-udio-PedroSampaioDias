// include/state.h
#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/mutex.h"
#include "config.h"

#define STATUS_LINE_SIZE 32

extern uint16_t       audio_buffer[BUFFER_SIZE];
extern volatile bool  recording;
extern volatile bool  playing;
extern volatile bool  ready;
extern volatile bool  buffer_has_audio;

extern volatile uint32_t sample_index;

extern mutex_t        display_mutex;
extern char           status_line[STATUS_LINE_SIZE];

#endif // STATE_H

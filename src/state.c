// src/state.c
#include "state.h"
#include "config.h"

uint16_t    audio_buffer[BUFFER_SIZE];

volatile uint32_t sample_index = 0;

volatile bool recording         = false;
volatile bool playing           = false;
volatile bool ready             = true;
volatile bool buffer_has_audio  = false;

mutex_t      display_mutex;
char         status_line[STATUS_LINE_SIZE] = "Audio Synthesizer";

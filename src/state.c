#include "state.h"
#include "config.h"

uint16_t    audio_buffer[BUFFER_SIZE];
volatile bool recording         = false;
volatile bool playing           = false;
volatile bool ready             = true;
volatile bool buffer_has_audio  = false;
volatile uint32_t sample_index  = 0;
volatile screen_mode_t current_screen = MENU_SCREEN;
mutex_t      display_mutex;
char         status_line[STATUS_LINE_SIZE] = "Audio Synthesizer";

uint16_t waveform_display[WAVEFORM_DISPLAY_SIZE];
volatile bool waveform_ready = false;

volatile bool initial_screen_active = true;

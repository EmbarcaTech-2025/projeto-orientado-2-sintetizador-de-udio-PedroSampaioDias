#include "audio.h"
#include "config.h"
#include "state.h"
#include "led.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

#define FILTER_WINDOW     3 
#define NOISE_THRESHOLD   250

void init_audio_pwm(void) {
    gpio_set_function(PWM_PIN_1, GPIO_FUNC_PWM);
    uint slice1 = pwm_gpio_to_slice_num(PWM_PIN_1);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 1.0f);
    pwm_config_set_wrap(&cfg, PWM_RESOLUTION);
    pwm_init(slice1, &cfg, true);

    gpio_set_function(PWM_PIN_2, GPIO_FUNC_PWM);
    uint slice2 = pwm_gpio_to_slice_num(PWM_PIN_2);
    pwm_init(slice2, &cfg, true);
}

void record_audio(void) {
    if (recording) return;

    recording = true;
    ready = false;
    buffer_has_audio = false;
    sample_index = 0;
    waveform_ready = false;
    current_screen = RECORDING_SCREEN;

    set_rgb_led(true, false, false);

    absolute_time_t next = get_absolute_time();
    
    uint16_t filter_buffer[FILTER_WINDOW] = {0};
    uint8_t filter_index = 0;
    uint32_t filter_sum = 0;
    bool first_samples = true;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        // 1. Leitura do ADC (com média de 8 amostras para reduzir ruído)
        uint32_t raw_sum = 0;
        for (int j = 0; j < 8; j++) raw_sum += adc_read();
        uint16_t raw_sample = raw_sum >> 3; // Divide por 8

        // 2. Aplica filtro de média móvel
        filter_sum -= filter_buffer[filter_index];       // Remove a amostra mais antiga
        filter_buffer[filter_index] = raw_sample;        // Armazena nova amostra
        filter_sum += raw_sample;                        // Adiciona ao somatório
        filter_index = (filter_index + 1) % FILTER_WINDOW; // Avança índice circular

        // 3. Calcula média e aplica threshold
        uint16_t filtered_sample;
        if (first_samples && i >= FILTER_WINDOW-1) {
            first_samples = false; // Janela cheia pela primeira vez
        }
        
        if (first_samples) {
            filtered_sample = raw_sample; // Usa valor bruto até encher a janela
        } else {
            filtered_sample = filter_sum / FILTER_WINDOW;
        }

        // 4. Remove ruído de fundo
        if (filtered_sample < NOISE_THRESHOLD) {
            filtered_sample = 0;
        } else {
            filtered_sample -= NOISE_THRESHOLD; // Remove offset
            // Limita o valor máximo
            if (filtered_sample > PWM_RESOLUTION) filtered_sample = PWM_RESOLUTION;
        }

        // 5. Armazena amostra filtrada
        audio_buffer[i] = filtered_sample;

        // Atualiza visualização (a cada N amostras)
        if (i % (BUFFER_SIZE / WAVEFORM_DISPLAY_SIZE) == 0 && 
            sample_index < WAVEFORM_DISPLAY_SIZE) {
            waveform_display[sample_index] = filtered_sample;
            sample_index++;
            waveform_ready = true;
        }

        next = delayed_by_us(next, SAMPLE_INTERVAL_US);
        busy_wait_until(next);
    }

    recording = false;
    buffer_has_audio = true;
    ready = true;

    sleep_ms(POST_ACTION_DELAY_MS);
    set_rgb_led(false, false, false);
    
    // Espera 2 segundos antes de mostrar a waveform
    sleep_ms(2000);
    current_screen = WAVEFORM_SCREEN;
}

void play_audio(void) {
    playing = true;
    current_screen = WAVEFORM_SCREEN;
    set_rgb_led(false, true, false);

    absolute_time_t next = get_absolute_time();
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        uint32_t sample = (uint32_t)(audio_buffer[i] * VOLUME_GAIN);
        if (sample > PWM_RESOLUTION) sample = PWM_RESOLUTION;
        pwm_set_gpio_level(PWM_PIN_1, sample);
        pwm_set_gpio_level(PWM_PIN_2, sample);
        next = delayed_by_us(next, SAMPLE_INTERVAL_US);
        busy_wait_until(next);
    }

    playing = false;
    sleep_ms(POST_ACTION_DELAY_MS);
    set_rgb_led(false, false, false);
    pwm_set_gpio_level(PWM_PIN_1, 0);
    pwm_set_gpio_level(PWM_PIN_2, 0);
}
// arquivo: audio.c
#include "audio.h"
#include "config.h"
#include "state.h"
#include "led.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

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
    recording        = true;
    ready            = false;
    buffer_has_audio = false;

    set_rgb_led(true, false, false);

    absolute_time_t next = get_absolute_time();
    for (int i = 0; i < BUFFER_SIZE; i++) {
        uint32_t sum = 0;
        for (int j = 0; j < 8; j++) sum += adc_read();
        audio_buffer[i] = sum >> 3;
        next = delayed_by_us(next, SAMPLE_INTERVAL_US);
        busy_wait_until(next);
    }

    recording        = false;
    buffer_has_audio = true;
    ready            = true;

    sleep_ms(POST_ACTION_DELAY_MS);
    set_rgb_led(false, false, false);
}

void play_audio(void) {
    playing = true;
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

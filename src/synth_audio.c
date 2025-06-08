// main.c
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "config.h"
#include "state.h"
#include "led.h"
#include "audio.h"
#include "display.h"
#include "input.h"

int main() {
    stdio_init_all();
    mutex_init(&display_mutex);
    multicore_launch_core1(core1_entry);

    adc_init();
    adc_gpio_init(28);
    adc_select_input(ADC_CHANNEL);
    adc_set_clkdiv(0);

    init_audio_pwm();

    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_RED,   GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(LED_BLUE,  GPIO_OUT);
    set_rgb_led(false, false, false);

    init_input();

    while (1) {
        if (recording) {
            set_rgb_led(true, false, false);
        } else if (playing) {
            set_rgb_led(false, true, false);
        } else {
            set_rgb_led(false, false, buffer_has_audio);
        }

        if (!recording && ready && is_button_a_pressed()) {
            sleep_ms(50);
            record_audio();
        } else if (!playing && ready && is_button_b_pressed()) {
            sleep_ms(50);
            play_audio();
        }

        sleep_ms(10);
    }

    return 0;
}

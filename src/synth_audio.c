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

    absolute_time_t last_button_press = nil_time;

    while (1) {
        if (recording) {
            set_rgb_led(true, false, false);  // Vermelho ao gravar
        } else if (playing && buffer_has_audio) {  // Só verde se tiver áudio
            set_rgb_led(false, true, false);
        } else {
            set_rgb_led(false, false, buffer_has_audio);  // Azul se tiver áudio
        }

        // Ignora botões se a tela inicial estiver ativa
        if (initial_screen_active) {
            sleep_ms(10);
            continue;
        }

        bool debounce_expired = is_nil_time(last_button_press) || 
                              absolute_time_diff_us(last_button_press, get_absolute_time()) > DEBOUNCE_DELAY_MS * 1000;

        if (current_screen == MENU_SCREEN && debounce_expired) {
            if (!recording && ready && is_button_a_pressed()) {
                last_button_press = get_absolute_time();
                record_audio();
            } 
            else if (!playing && is_button_b_pressed()) {  // Removida verificação do ready
                last_button_press = get_absolute_time();
                current_screen = WAVEFORM_SCREEN;  // Apenas muda para tela de waveform
            }
        } 
        else if (current_screen == WAVEFORM_SCREEN && debounce_expired) {
            if (is_button_a_pressed()) {
                last_button_press = get_absolute_time();
                current_screen = MENU_SCREEN;
            } 
            else if (is_button_b_pressed() && !playing && buffer_has_audio) {  // Só reproduz se tiver áudio
                last_button_press = get_absolute_time();
                play_audio();
            }
        }

        sleep_ms(10);
    }

    return 0;
}
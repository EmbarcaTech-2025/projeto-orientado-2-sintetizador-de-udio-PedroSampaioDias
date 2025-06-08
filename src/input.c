// arquivo: input.c
#include "input.h"
#include "hardware/gpio.h"
#include "config.h"
#include "pico/stdlib.h"

void init_input(void) {
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);
}

bool is_button_a_pressed(void) {
    return !gpio_get(BUTTON_A);
}

bool is_button_b_pressed(void) {
    return !gpio_get(BUTTON_B);
}

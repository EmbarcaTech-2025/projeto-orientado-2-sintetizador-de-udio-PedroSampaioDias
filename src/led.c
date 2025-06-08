// arquivo: led.c
#include "led.h"
#include "hardware/gpio.h"
#include "config.h"

void set_rgb_led(bool red, bool green, bool blue) {
    gpio_put(LED_RED,   red);
    gpio_put(LED_GREEN, green);
    gpio_put(LED_BLUE,  blue);
}

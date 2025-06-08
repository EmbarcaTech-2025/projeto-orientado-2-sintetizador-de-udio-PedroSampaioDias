// arquivo: config.h
#ifndef CONFIG_H
#define CONFIG_H

// Áudio
#define SAMPLE_RATE            22000
#define SAMPLE_INTERVAL_US     (1000000 / SAMPLE_RATE)
#define AUDIO_DURATION         5
#define BUFFER_SIZE            (SAMPLE_RATE * AUDIO_DURATION)
#define ADC_CHANNEL            2
#define PWM_PIN_1              21
#define PWM_PIN_2              10
#define VOLUME_GAIN            1.0f
#define PWM_RESOLUTION         4095
#define POST_ACTION_DELAY_MS   500

// Botões e LEDs
#define BUTTON_A               5
#define BUTTON_B               6
#define LED_RED                13
#define LED_GREEN              11
#define LED_BLUE               12

// OLED
#define SDA_OLED               14
#define SCL_OLED               15
#define I2C_PORT               i2c1
#define OLED_WIDTH             128
#define OLED_HEIGHT            64
#define OLED_ADDRESS           0x3C

#endif // CONFIG_H

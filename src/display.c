#include "display.h"
#include "config.h"
#include "state.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "ssd1306.h"
#include <stdio.h>
#include <string.h>

#define MENU_OPTION_SPACING 4
#define CHAR_WIDTH          6
#define WAVEFORM_MAX_HEIGHT (OLED_HEIGHT - 25)  // Altura máxima (OLED_HEIGHT - espaço para textos)
#define TEXT_AREA_HEIGHT    20                  // Área reservada para textos

static void draw_menu(ssd1306_t *disp) {
    ssd1306_clear(disp);
    
    const uint8_t rect_w = 100;
    const uint8_t rect_h = 2 * 12 + MENU_OPTION_SPACING + 4;
    const uint8_t rect_x = (OLED_WIDTH  - rect_w) / 2;
    const uint8_t rect_y = (OLED_HEIGHT - rect_h) / 2;

    ssd1306_draw_empty_square(disp, rect_x, rect_y, rect_w, rect_h);

    const char *opt1 = "A - Gravar";
    const char *opt2 = "B - Visualizar";

    uint8_t w1 = strlen(opt1) * CHAR_WIDTH;
    uint8_t w2 = strlen(opt2) * CHAR_WIDTH;

    uint8_t text_x1 = rect_x + (rect_w - w1) / 2;
    uint8_t text_x2 = rect_x + (rect_w - w2) / 2;

    uint8_t total_text_h = 2 * 8 + MENU_OPTION_SPACING;
    uint8_t offset_y     = rect_y + (rect_h - total_text_h) / 2;
    uint8_t text_y1      = offset_y;
    uint8_t text_y2      = offset_y + 8 + MENU_OPTION_SPACING;

    ssd1306_draw_string(disp, text_x1, text_y1, 1, opt1);
    ssd1306_draw_string(disp, text_x2, text_y2, 1, opt2);
}

static void draw_recording(ssd1306_t *disp) {
    ssd1306_clear(disp);
    ssd1306_draw_empty_square(disp, 0, 0, OLED_WIDTH - 1, OLED_HEIGHT - 1);
    ssd1306_draw_string(disp, 30, 2, 1, "Gravando...");
    
    if (waveform_ready && sample_index > 0) {
        for (uint32_t i = 0; i < sample_index; i++) {
            uint16_t sample = waveform_display[i];
            uint8_t height = (sample * WAVEFORM_MAX_HEIGHT) / 4095;
            uint8_t y_pos = OLED_HEIGHT - 2 - height;
            if (i < OLED_WIDTH) {
                ssd1306_draw_line(disp, i, y_pos, i, OLED_HEIGHT - 2);
            }
        }
    }
}

static void draw_waveform(ssd1306_t *disp) {
    ssd1306_clear(disp);
    ssd1306_draw_empty_square(disp, 0, 0, OLED_WIDTH - 1, OLED_HEIGHT - 1);
    
    if (buffer_has_audio) {
        // Textos informativos
        ssd1306_draw_string(disp, 5, 2, 1, "Onda");
        ssd1306_draw_string(disp, OLED_WIDTH - 65, 2, 1, "A: Menu");
        ssd1306_draw_string(disp, OLED_WIDTH - 80, 12, 1, "B: Reproduzir");
        
        // Desenha a forma de onda
        for (uint32_t i = 0; i < WAVEFORM_DISPLAY_SIZE; i++) {
            uint16_t sample = waveform_display[i];
            uint8_t height = (sample * WAVEFORM_MAX_HEIGHT) / 4095;
            uint8_t y_pos = OLED_HEIGHT - 2 - height;
            
            if (i < OLED_WIDTH) {
                ssd1306_draw_line(disp, i, y_pos, i, OLED_HEIGHT - 2);
            }
        }
    } else {
        // Mensagem quando não há áudio
        const char *msg = "Nenhum audio gravado";
        uint8_t msg_width = strlen(msg) * CHAR_WIDTH;
        uint8_t msg_x = (OLED_WIDTH - msg_width) / 2;
        uint8_t msg_y = (OLED_HEIGHT - 8) / 2;
        
        ssd1306_draw_string(disp, msg_x, msg_y, 1, msg);
        
        // Mostra apenas a opção de voltar ao menu
        ssd1306_draw_string(disp, OLED_WIDTH - 45, 2, 1, "A: Menu");
    }
}

void core1_entry(void) {
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_OLED, GPIO_FUNC_I2C);
    gpio_set_function(SCL_OLED, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_OLED);
    gpio_pull_up(SCL_OLED);

    ssd1306_t disp;
    disp.external_vcc = false;
    if (!ssd1306_init(&disp, OLED_WIDTH, OLED_HEIGHT, OLED_ADDRESS, I2C_PORT)) {
        while (1) tight_loop_contents();
    }

    // Tela inicial (sem interação)
    ssd1306_clear(&disp);
    ssd1306_draw_empty_square(&disp, 0, 0, OLED_WIDTH - 1, OLED_HEIGHT - 1);
    int title_y = (OLED_HEIGHT - 8) / 2;
    ssd1306_draw_string(&disp, 15, title_y, 1, "Audio Synthesizer");
    ssd1306_show(&disp);
    
    sleep_ms(1500);  // Mostra por 1.5 segundos
    initial_screen_active = false;
    ssd1306_clear(&disp);
    ssd1306_show(&disp);

    // Loop principal de renderização
    while (1) {
        mutex_enter_blocking(&display_mutex);
        
        switch(current_screen) {
            case MENU_SCREEN:
                draw_menu(&disp);
                break;
                
            case RECORDING_SCREEN:
                draw_recording(&disp);
                break;
                
            case WAVEFORM_SCREEN:
                draw_waveform(&disp);
                break;
        }
        
        ssd1306_show(&disp);
        mutex_exit(&display_mutex);
        sleep_ms(50);
    }
}
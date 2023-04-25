RGB_MATRIX_EFFECT(KUDD_CUSTOM)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#include "print.h"

/*
struct key_color
{
    uint8_t h;
    uint8_t s;
    uint8_t on;
    uint32_t last_tick;
};
static struct key_color color_map[RGB_MATRIX_LED_COUNT];
*/

extern struct key_color color_map[MATRIX_ROWS][MATRIX_COLS];

uint32_t diff_tick(uint32_t prev, uint32_t now) {
    return (prev <= now) ? now - prev : (UINT32_MAX - prev) + now + 1;
}

void populate_map(void)
{
    //srand(chVTGetSystemTimeX());
    srand(timer_read32());
    //RGB_MATRIX_USE_LIMITS(led_min, led_max);
    //for (uint8_t i = led_min; i < led_max; i++)
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            //uint8_t h = rand() % 256;
            color_map[row][col].h = 0;
            color_map[row][col].s = 0;
            color_map[row][col].on = 0;
            color_map[row][col].last_tick = 0;
        }
    }
}

// e.g: A simple effect, self-contained within a single method
bool KUDD_CUSTOM(effect_params_t* params) {
    if (params->init) populate_map();
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // speed = 0 ~ 255

    uint32_t now = timer_read32();
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            if (color_map[row][col].on == 0) {
                // first keydown
                color_map[row][col].on = 1;
                color_map[row][col].h = rand() % UINT8_MAX;
                color_map[row][col].s = rand() % UINT8_MAX;
                color_map[row][col].last_tick = timer_read32();
            }
            else {
                // keydown again
                color_map[row][col].last_tick = timer_read32();
            }

            uint32_t diff = 0;
            uint8_t brightness = 0;

            if (color_map[row][col].on != 0) {
                diff = diff_tick(color_map[row][col].last_tick, now);
                if (diff > 0 && diff < UINT8_MAX) {
                    brightness = UINT8_MAX - diff;
                }
            }

            if (brightness == 0) {
                color_map[row][col].on = 0;
            }

            HSV hsv = { .h = color_map[row][col].h, .s = color_map[row][col].s, .v = brightness };
            RGB rgb = hsv_to_rgb(hsv);

            static uint32_t counter = 0;
            if (brightness > 0) {
                if (counter++ % 10 == 0) {
                    //uprintf("[%d] custom effect.. t : %u, n : %u, diff : %u h : %d, b : %d, rgb = %d %d %d\n", i, color_map[row][col].last_tick, now, diff, color_map[row][col].h, brightness, rgb.r, rgb.g, rgb.b);
                }
            }

            if (g_led_config.matrix_co[row][col] >= led_min && g_led_config.matrix_co[row][col] < led_max) {
                rgb_matrix_set_color(g_led_config.matrix_co[row][col], rgb.r, rgb.g, rgb.b);
            }
        }
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS

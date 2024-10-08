#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "xtensa/core-macros.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "flipdot.h"

static const char *TAG = "FD";

static bool dotboard_synced = false;
static dotboard_t last_written_dotboard;

/**
 * Initialse the Flipdot Controller interface.
 */
void flipdot_init()
{
    // Set up the GPIO pins for the signalling
    ESP_LOGI(TAG, "setting up GPIO pins");

    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    // All outputs

    gpio_set_direction(PIN_ENABLE_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_ENABLE_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_ENABLE_3, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_ENABLE_4, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_COLUMN_ADVANCE, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_RESET, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_SET_UNSET, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_ROW_ADVANCE, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_COIL_DRIVE, GPIO_MODE_OUTPUT);

    // All pins start low
    gpio_set_level(PIN_ENABLE_1, 0);
    gpio_set_level(PIN_ENABLE_2, 0);
    gpio_set_level(PIN_ENABLE_3, 0);
    gpio_set_level(PIN_ENABLE_4, 0);
    gpio_set_level(PIN_COLUMN_ADVANCE, 0);
    gpio_set_level(PIN_RESET, 0);
    gpio_set_level(PIN_SET_UNSET, 0);
    gpio_set_level(PIN_ROW_ADVANCE, 0);
    gpio_set_level(PIN_COIL_DRIVE, 0);

    // Create space for a backing store for the last-written dotboard
    for (uint c = 0; c < DOT_COLUMNS; c++)
    {
        for (uint r = 0; r < DOT_ROWS; r++)
        {
            last_written_dotboard[c][r] = 0;
        }
    }
}

// Insert a wait
static void wait(uint wait_us)
{
    uint32_t c_current = 0, c_start = XTHAL_GET_CCOUNT();
    do
    {
        c_current = XTHAL_GET_CCOUNT();
    } while (c_current - c_start < CYCLES_PER_US * wait_us);
}

// Setup the dot state
static void setup_state(bool dot_state)
{
    gpio_set_level(PIN_SET_UNSET, dot_state);
}

// Reset
static void reset()
{
    gpio_set_level(PIN_RESET, 1);
    wait(WAIT_TIME);
    gpio_set_level(PIN_RESET, 0);
}

// Advance 1 row
static void row_advance()
{
    gpio_set_level(PIN_ROW_ADVANCE, 1);
    wait(WAIT_TIME);
    gpio_set_level(PIN_ROW_ADVANCE, 0);
}

// Advance 1 column
static void column_advance()
{
    gpio_set_level(PIN_COLUMN_ADVANCE, 1);
    wait(WAIT_TIME);
    gpio_set_level(PIN_COLUMN_ADVANCE, 0);
}

// Pulse the coil
static void pulse_coil()
{
    gpio_set_level(PIN_COIL_DRIVE, 1);
    wait(PULSE_TIME_US);
    gpio_set_level(PIN_COIL_DRIVE, 0);
}

// Enable
static void enable(int pin)
{   
    switch (pin)
    {
    case 1:
        gpio_set_level(PIN_ENABLE_1, 1);
        break;
    case 2:
        gpio_set_level(PIN_ENABLE_2, 1);
        break;
    case 3:
        gpio_set_level(PIN_ENABLE_3, 1);
        break;
    case 4:
        gpio_set_level(PIN_ENABLE_4, 1);
    default:
        break;
    }
}

static void disable(int pin)
{
    switch (pin)
    {
    case 1:
        gpio_set_level(PIN_ENABLE_1, 0);
        break;
    case 2:
        gpio_set_level(PIN_ENABLE_2, 0);
        break;
    case 3:
        gpio_set_level(PIN_ENABLE_3, 0);
        break;
    case 4:
        gpio_set_level(PIN_ENABLE_4, 0);
    default:
        break;
    }
}

static int get_current_panel(int column)
{
    if(column < COLUMNS_PER_PANEL) {
        return 1;
    } else if (column >= COLUMNS_PER_PANEL && column < COLUMNS_PER_PANEL * 2) {
        return 2;
    } else if (column >= COLUMNS_PER_PANEL * 2 && column < COLUMNS_PER_PANEL * 3) {
        return 3; 
    } else {
        return 4;
    }
}

/**
 * Write an entire dotboard.
 * 
 * Only dots that need to change state will be pulsed unless is_keyframe is stated.
 * In calling code, I'd ensure is_keyframe is occasionally set in case the dotboard is
 * ever disconnected from the controller for a moment.
 */
void write_dotboard(dotboard_t *dots, bool is_keyframe)
{

    // Force an update if we don't have a dotboard buffer yet
    is_keyframe = dotboard_synced ? is_keyframe : true;

    // For each column
    for (uint c = 0; c < DOT_COLUMNS; c++)
    {
        int current_panel = get_current_panel(c);
        enable(current_panel);
        wait(WAIT_TIME);
        // For each row in the column...
        for (uint r = 0; r < DOT_ROWS; r++)
        {

            // Skip dots that don't need updating
            if (is_keyframe || (*dots)[c][r] != last_written_dotboard[c][r])
            {

                // Set up the set/unset pin
                setup_state((*dots)[c][r]);
                wait(WAIT_TIME);

                // Pulse the coil drive
                pulse_coil();
            }

            // Advance the row
            row_advance();
            wait(WAIT_TIME);
        }

        // Advance the column
        column_advance();
        wait(WAIT_TIME);
        disable(current_panel);
        wait(WAIT_TIME);
    }

    // Update the buffer with the new dotboard
    for (uint c = 0; c < DOT_COLUMNS; c++)
    {
        for (uint r = 0; r < DOT_ROWS; r++)
        {
            last_written_dotboard[c][r] = (*dots)[c][r];
        }
    }

    dotboard_synced = true;
}

#ifndef FLIPDOT_H
#define FLIPDOT_H

#include <stdint.h>
#include <stdbool.h>

// Dimensions of your display
#define DOT_ROWS 8
#define DOT_COLUMNS 84
#define NUM_PANELS 3
#define COLUMNS_PER_PANEL 28

// GPIO Pin Definitions
#define PIN_ENABLE_1 GPIO_NUM_12
#define PIN_ENABLE_2 GPIO_NUM_13
#define PIN_ENABLE_3 GPIO_NUM_14
#define PIN_ENABLE_4 GPIO_NUM_15
#define PIN_COLUMN_ADVANCE GPIO_NUM_17
#define PIN_RESET GPIO_NUM_16
#define PIN_SET_UNSET GPIO_NUM_19
#define PIN_ROW_ADVANCE GPIO_NUM_18
#define PIN_COIL_DRIVE GPIO_NUM_21

#define GPIO_OUTPUT_PIN_SEL ((1ULL << PIN_ENABLE_1) | (1ULL << PIN_ENABLE_2) | (1ULL << PIN_ENABLE_3) | (1ULL << PIN_ENABLE_4) | (1ULL << PIN_COLUMN_ADVANCE) | (1ULL << PIN_RESET) | (1ULL << PIN_SET_UNSET) | (1ULL << PIN_ROW_ADVANCE) | (1ULL << PIN_COIL_DRIVE))

// Cycle count in 1 µS
#define CYCLES_PER_US 600

// Timings (µS)
#define PULSE_TIME_US 200

// Types
typedef uint8_t dotboard_t[DOT_COLUMNS][DOT_ROWS];

// Public subs
void flipdot_init();
void write_dotboard(dotboard_t *dots, bool is_keyframe);

#endif
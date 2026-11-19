#include "MK64F12.h"
#include <stdint.h>

#define TIMER_START_SECONDS 30U

// LED
#define LED1_PIN (1U << 1)    // PTD1
#define LED2_PIN (1U << 3)    // PTD3

// Switch
#define SW1_PIN (1U << 3)     // PTB3
#define SW2_PIN (1U << 10)    // PTB10

// 7-Segment
#define SEG_LATCH_PIN (1U << 23)  // PTB23
#define SEG_CLOCK_PIN (1U << 3)   // PTC3
#define SEG_DATA_PIN  (1U << 12)  // PTC12

// Buzzer
#define BUZZER_PIN (1U << 1)     // PTA1

// LDR
#define LDR_PIN (1U << 2)        // PTA2

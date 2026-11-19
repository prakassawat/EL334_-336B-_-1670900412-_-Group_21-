
#include "MK64F12.h"
#include <stdint.h>

// ==========================================
// ????????????????
// ==========================================

#define TIMER_START_SECONDS  30U

// ==========================================
// LED ?? Multi-Function Shield
// ==========================================

#define LED1_PIN (1U << 1)    // PTD1
#define LED2_PIN (1U << 3)    // PTD3

// ==========================================
// Switch
// ==========================================

#define SW1_PIN (1U << 3)     // PTB3
#define SW2_PIN (1U << 10)    // PTB10

// ==========================================
// 7-Segment
// ==========================================

#define SEG_LATCH_PIN (1U << 23)  // PTB23
#define SEG_CLOCK_PIN (1U << 3)   // PTC3
#define SEG_DATA_PIN  (1U << 12)  // PTC12

// ==========================================
// Buzzer
// ==========================================

#define BUZZER_PIN (1U << 1)     // PTA1

// ==========================================
// LDR Module
// ==========================================

// D5 = PTA2
#define LDR_PIN (1U << 2)

// ==========================================
// ?????????????????
// ==========================================

typedef enum
{
    MODE_AUTO = 0U,
    MODE_MANUAL,
    MODE_TIMER
} SystemMode;

// ==========================================
// ?????????? 7-Segment
// ==========================================

const uint8_t SEGMENT_CODE[10] =
{
    0xC0,  // 0
    0xF9,  // 1
    0xA4,  // 2
    0xB0,  // 3
    0x99,  // 4
    0x92,  // 5
    0x82,  // 6
    0xF8,  // 7
    0x80,  // 8
    0x90   // 9
};

// ???????????? 7-Segment
const uint8_t DIGIT_SELECT[4] =
{
    0x01,
    0x02,
    0x04,
    0x08
};

// ==========================================
// ??????????
// ==========================================

// ???????????? Switch
volatile uint8_t sw1_event = 0U;
volatile uint8_t sw2_event = 0U;

// ?????? Timer
volatile uint32_t countdown_seconds = TIMER_START_SECONDS;

volatile uint8_t timer_running = 0U;
volatile uint8_t timer_finished_event = 0U;

// ????? Alarm
uint8_t alarm_active = 0U;

// ????????? Manual Mode
uint8_t manual_led1_state = 0U;

// ????????????
SystemMode system_mode = MODE_AUTO;

// ??????? Bus Clock
uint32_t pit_bus_clock_hz = 0U;

// ==========================================
// FUNCTION PROTOTYPES
// ==========================================

void LED_Init(void);
void LED1_On(void);
void LED1_Off(void);
void LED2_On(void);
void LED2_Off(void);

void Switch_Init(void);

void SevenSeg_Init(void);
void SevenSeg_ShiftOut(uint8_t data);
void SevenSeg_Refresh(void);

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);

void LDR_Init(void);

void System_UpdateOutputs(void);

uint32_t GetBusClockHz(void);
void PIT_Init(void);
void Timer_Start(void);
void Timer_Stop(void);

// ==========================================
// LED MODULE
// ==========================================

void LED_Init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    PORTD->PCR[1] = PORT_PCR_MUX(1);
    PORTD->PCR[3] = PORT_PCR_MUX(1);

    // LED ??? Active LOW
    PTD->PSOR = LED1_PIN | LED2_PIN;

    // ?????????? Output
    PTD->PDDR |= LED1_PIN | LED2_PIN;
}

void LED1_On(void)
{
    PTD->PCOR = LED1_PIN;
}

void LED1_Off(void)
{
    PTD->PSOR = LED1_PIN;
}

void LED2_On(void)
{
    PTD->PCOR = LED2_PIN;
}

void LED2_Off(void)
{
    PTD->PSOR = LED2_PIN;
}

// ==========================================
// SWITCH MODULE
// ==========================================

void Switch_Init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // S1 = PTB3
    PORTB->PCR[3] =
        PORT_PCR_MUX(1) |
        PORT_PCR_PE_MASK |
        PORT_PCR_PS_MASK |
        PORT_PCR_IRQC(0xA);

    // S2 = PTB10
    PORTB->PCR[10] =
        PORT_PCR_MUX(1) |
        PORT_PCR_PE_MASK |
        PORT_PCR_PS_MASK |
        PORT_PCR_IRQC(0xA);

    // ?????????? Input
    PTB->PDDR &= ~(SW1_PIN | SW2_PIN);

    // ??????? Interrupt Flag
    PORTB->ISFR = SW1_PIN | SW2_PIN;

    NVIC_ClearPendingIRQ(PORTB_IRQn);
    NVIC_EnableIRQ(PORTB_IRQn);
}

// ==========================================
// PORTB INTERRUPT
// ==========================================

void PORTB_IRQHandler(void)
{
    uint32_t status;

    status = PORTB->ISFR;

    if ((status & SW1_PIN) != 0U)
    {
        sw1_event = 1U;
    }

    if ((status & SW2_PIN) != 0U)
    {
        sw2_event = 1U;
    }

    // ??????? Interrupt Flag
    PORTB->ISFR = status & (SW1_PIN | SW2_PIN);
}

// ==========================================
// 7-SEGMENT MODULE
// ==========================================

void SevenSeg_Init(void)
{
    SIM->SCGC5 |=
        SIM_SCGC5_PORTB_MASK |
        SIM_SCGC5_PORTC_MASK;

    // ?? Latch
    PORTB->PCR[23] = PORT_PCR_MUX(1);

    // ?? Clock ??? Data
    PORTC->PCR[3] = PORT_PCR_MUX(1);
    PORTC->PCR[12] = PORT_PCR_MUX(1);

    // ?????????? Output
    PTB->PDDR |= SEG_LATCH_PIN;

    PTC->PDDR |=
        SEG_CLOCK_PIN |
        SEG_DATA_PIN;

    PTB->PSOR = SEG_LATCH_PIN;

    PTC->PCOR =
        SEG_CLOCK_PIN |
        SEG_DATA_PIN;
}

// ????????? 8 ???????? 74HC595
void SevenSeg_ShiftOut(uint8_t data)
{
    uint8_t i;

    for (i = 0U; i < 8U; i++)
    {
        if ((data & 0x80U) != 0U)
        {
            PTC->PSOR = SEG_DATA_PIN;
        }
        else
        {
            PTC->PCOR = SEG_DATA_PIN;
        }

        // Clock Pulse
        PTC->PSOR = SEG_CLOCK_PIN;
        PTC->PCOR = SEG_CLOCK_PIN;

        data <<= 1U;
    }
}

// ??????????????? TIMER MODE
void SevenSeg_Refresh(void)
{
    static uint8_t current_digit = 0U;

    uint32_t display_value;
    uint8_t number;
    uint8_t segment_data;

    volatile uint32_t delay_count;

    if (system_mode == MODE_TIMER)
    {
        display_value = countdown_seconds;

        switch (current_digit)
        {
            case 0U:
                number = (uint8_t)((display_value / 1000U) % 10U);
                break;

            case 1U:
                number = (uint8_t)((display_value / 100U) % 10U);
                break;

            case 2U:
                number = (uint8_t)((display_value / 10U) % 10U);
                break;

            default:
                number = (uint8_t)(display_value % 10U);
                break;
        }

        segment_data = SEGMENT_CODE[number];
    }
    else
    {
        // ???????????????????????? TIMER MODE
        segment_data = 0xFFU;
    }

    // ?????????????? 7-Segment
    PTB->PCOR = SEG_LATCH_PIN;

    SevenSeg_ShiftOut(segment_data);

    SevenSeg_ShiftOut(DIGIT_SELECT[current_digit]);

    PTB->PSOR = SEG_LATCH_PIN;

    current_digit++;

    if (current_digit >= 4U)
    {
        current_digit = 0U;
    }

    // ?????????????????
    for (delay_count = 0U; delay_count < 8000U; delay_count++)
    {
        __NOP();
    }
}

// ==========================================
// BUZZER MODULE
// ==========================================

void Buzzer_Init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    PORTA->PCR[1] = PORT_PCR_MUX(1);

    // ???????????? Input
    PTA->PDDR &= ~BUZZER_PIN;
}

void Buzzer_On(void)
{
    PTA->PCOR = BUZZER_PIN;

    PTA->PDDR |= BUZZER_PIN;
}

void Buzzer_Off(void)
{
    PTA->PDDR &= ~BUZZER_PIN;
}

// ==========================================
// LDR MODULE
// ==========================================

void LDR_Init(void)
{
    // ???? Clock ??? PORTA
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    // D5 = PTA2 ???????? GPIO
    PORTA->PCR[2] = PORT_PCR_MUX(1);

    // ???????? Input
    PTA->PDDR &= ~LDR_PIN;
}

// ==========================================
// ?????????????????????
// ==========================================

void System_UpdateOutputs(void)
{
    // --------------------------------------
    // AUTO MODE
    // --------------------------------------

    if (system_mode == MODE_AUTO)
    {
        // LED2 ??? ??????????? AUTO MODE
        LED2_On();

        // ??????????????:
        // D0 HIGH = ???????
        // D0 LOW  = ??????

        if ((PTA->PDIR & LDR_PIN) != 0U)
        {
            // ??????? ???? LED1
            LED1_On();
        }
        else
        {
            // ?????? ??? LED1
            LED1_Off();
        }
    }

    // --------------------------------------
    // MANUAL MODE
    // --------------------------------------

    else if (system_mode == MODE_MANUAL)
    {
        // LED2 ??? ????????????????? AUTO
        LED2_Off();

        // LED1 ???????????????????????????
        if (manual_led1_state != 0U)
        {
            LED1_On();
        }
        else
        {
            LED1_Off();
        }
    }

    // --------------------------------------
    // TIMER MODE
    // --------------------------------------

    else if (system_mode == MODE_TIMER)
    {
        // LED2 ??? ??????????????? AUTO
        LED2_Off();

        if ((timer_running != 0U) &&
            (alarm_active == 0U))
        {
            // ???? LED1 ??????????????
            LED1_On();
        }
        else
        {
            // ??? LED1 ????????????????????
            LED1_Off();
        }
    }

    // --------------------------------------
    // BUZZER
    // --------------------------------------

    if (alarm_active != 0U)
    {
        Buzzer_On();
    }
    else
    {
        Buzzer_Off();
    }
}

// ==========================================
// PIT HARDWARE TIMER
// ==========================================

// ??????? Bus Clock
uint32_t GetBusClockHz(void)
{
    uint32_t outdiv1;
    uint32_t outdiv2;

    outdiv1 =
        ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)
        >> SIM_CLKDIV1_OUTDIV1_SHIFT) + 1U;

    outdiv2 =
        ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK)
        >> SIM_CLKDIV1_OUTDIV2_SHIFT) + 1U;

    return (uint32_t)
        (((uint64_t)SystemCoreClock * outdiv1) / outdiv2);
}

// ??????? PIT
void PIT_Init(void)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

    PIT->MCR = PIT_MCR_FRZ_MASK;

    SystemCoreClockUpdate();

    pit_bus_clock_hz = GetBusClockHz();

    if (pit_bus_clock_hz == 0U)
    {
        while (1)
        {
            // ??????? Clock ??????????
        }
    }

    // ??????? Interrupt ??? 1 ??????
    PIT->CHANNEL[0].LDVAL = pit_bus_clock_hz - 1U;

    PIT->CHANNEL[0].TCTRL = 0U;

    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

    NVIC_ClearPendingIRQ(PIT0_IRQn);
    NVIC_EnableIRQ(PIT0_IRQn);
}

// ????? Timer
void Timer_Start(void)
{
    __disable_irq();

    countdown_seconds = TIMER_START_SECONDS;

    timer_finished_event = 0U;
    timer_running = 1U;

    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

    PIT->CHANNEL[0].TCTRL =
        PIT_TCTRL_TIE_MASK |
        PIT_TCTRL_TEN_MASK;

    __enable_irq();
}

// ???? Timer
void Timer_Stop(void)
{
    __disable_irq();

    PIT->CHANNEL[0].TCTRL = 0U;

    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

    timer_running = 0U;
    timer_finished_event = 0U;

    __enable_irq();
}

// ==========================================
// PIT INTERRUPT
// ==========================================

void PIT0_IRQHandler(void)
{
    // ??????? Interrupt Flag
    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

    if (timer_running != 0U)
    {
        if (countdown_seconds > 0U)
        {
            countdown_seconds--;
        }

        if (countdown_seconds == 0U)
        {
            PIT->CHANNEL[0].TCTRL = 0U;

            timer_running = 0U;

            timer_finished_event = 1U;
        }
    }
}

// ==========================================
// MAIN PROGRAM
// ==========================================

int main(void)
{
    // ???????????????
    LED_Init();

    Switch_Init();

    SevenSeg_Init();

    Buzzer_Init();

    LDR_Init();

    PIT_Init();

    // ??????????????????????
    countdown_seconds = TIMER_START_SECONDS;

    timer_running = 0U;
    timer_finished_event = 0U;
    alarm_active = 0U;

    manual_led1_state = 0U;

    // ??????????? AUTO MODE
    system_mode = MODE_AUTO;

    // ??????????????????
    System_UpdateOutputs();

    // ======================================
    // MAIN LOOP
    // ======================================

    while (1)
    {
        // ?????? 7-Segment
        SevenSeg_Refresh();

        // ----------------------------------
        // ??????? Timer ???????
        // ----------------------------------

        if (timer_finished_event != 0U)
        {
            __disable_irq();

            timer_finished_event = 0U;

            __enable_irq();

            // ???? Alarm ????? Timer ???????
            alarm_active = 1U;
        }

        // ----------------------------------
        // ??????? S1 ???????????
        // ----------------------------------

        if (sw1_event != 0U)
        {
            __disable_irq();

            sw1_event = 0U;

            __enable_irq();

            // AUTO -> MANUAL
            if (system_mode == MODE_AUTO)
            {
                system_mode = MODE_MANUAL;

                // ????? MANUAL ?????????
                manual_led1_state = 0U;
            }

            // MANUAL -> TIMER
            else if (system_mode == MODE_MANUAL)
            {
                system_mode = MODE_TIMER;

                Timer_Stop();

                countdown_seconds = TIMER_START_SECONDS;

                alarm_active = 0U;
            }

            // TIMER -> AUTO
            else
            {
                Timer_Stop();

                countdown_seconds = TIMER_START_SECONDS;

                alarm_active = 0U;

                system_mode = MODE_AUTO;
            }
        }

        // ----------------------------------
        // ??????? S2
        // ----------------------------------

        if (sw2_event != 0U)
        {
            __disable_irq();

            sw2_event = 0U;

            __enable_irq();

            // S2 ?? MANUAL MODE
            if (system_mode == MODE_MANUAL)
            {
                // ?????????????? LED1
                manual_led1_state ^= 1U;
            }

            // S2 ?? TIMER MODE
            else if (system_mode == MODE_TIMER)
            {
                // ??? Alarm ??????????
                if (alarm_active != 0U)
                {
                    alarm_active = 0U;

                    Timer_Stop();

                    countdown_seconds = TIMER_START_SECONDS;
                }

                // ??? Timer ????????
                else if (timer_running != 0U)
                {
                    // ?????? Timer
                    Timer_Stop();

                    countdown_seconds = TIMER_START_SECONDS;
                }

                // ??? Timer ???????????
                else
                {
                    // ???????? 30 ??????
                    Timer_Start();
                }
            }
        }

        // ----------------------------------
        // ?????? LED ??? Buzzer ???????
        // ----------------------------------

        System_UpdateOutputs();
    }
}

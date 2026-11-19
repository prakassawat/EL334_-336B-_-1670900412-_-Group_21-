void SevenSeg_Init(void)
{
    SIM->SCGC5 |=
        SIM_SCGC5_PORTB_MASK |
        SIM_SCGC5_PORTC_MASK;

    // Latch
    PORTB->PCR[23] = PORT_PCR_MUX(1);

    // Clock and Data
    PORTC->PCR[3] = PORT_PCR_MUX(1);
    PORTC->PCR[12] = PORT_PCR_MUX(1);

    // Set Output
    PTB->PDDR |= SEG_LATCH_PIN;

    PTC->PDDR |=
        SEG_CLOCK_PIN |
        SEG_DATA_PIN;

    PTB->PSOR = SEG_LATCH_PIN;

    PTC->PCOR =
        SEG_CLOCK_PIN |
        SEG_DATA_PIN;
}


// Shift Out 8 Bits to 74HC595

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


// Display Timer Mode

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
        // Turn Off Display Outside TIMER MODE
        segment_data = 0xFFU;
    }

    // Update 7-Segment
    PTB->PCOR = SEG_LATCH_PIN;

    SevenSeg_ShiftOut(segment_data);

    SevenSeg_ShiftOut(DIGIT_SELECT[current_digit]);

    PTB->PSOR = SEG_LATCH_PIN;

    current_digit++;

    if (current_digit >= 4U)
    {
        current_digit = 0U;
    }

    // Small Delay
    for (delay_count = 0U; delay_count < 8000U; delay_count++)
    {
        __NOP();
    }
}

// Get Bus Clock

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


// Initialize PIT

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
            // Clock Error
        }
    }

    // Interrupt every 1 second
    PIT->CHANNEL[0].LDVAL = pit_bus_clock_hz - 1U;

    PIT->CHANNEL[0].TCTRL = 0U;

    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

    NVIC_ClearPendingIRQ(PIT0_IRQn);
    NVIC_EnableIRQ(PIT0_IRQn);
}


// Start Timer

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


// Stop Timer

void Timer_Stop(void)
{
    __disable_irq();

    PIT->CHANNEL[0].TCTRL = 0U;

    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

    timer_running = 0U;
    timer_finished_event = 0U;

    __enable_irq();
}

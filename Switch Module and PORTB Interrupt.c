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

    // Set Input
    PTB->PDDR &= ~(SW1_PIN | SW2_PIN);

    // Clear Interrupt Flag
    PORTB->ISFR = SW1_PIN | SW2_PIN;

    NVIC_ClearPendingIRQ(PORTB_IRQn);
    NVIC_EnableIRQ(PORTB_IRQn);
}


// PORTB INTERRUPT

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

    // Clear Interrupt Flag
    PORTB->ISFR = status & (SW1_PIN | SW2_PIN);
}

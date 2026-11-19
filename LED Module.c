void LED_Init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    PORTD->PCR[1] = PORT_PCR_MUX(1);
    PORTD->PCR[3] = PORT_PCR_MUX(1);

    // LED Active LOW
    PTD->PSOR = LED1_PIN | LED2_PIN;

    // Set Output
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

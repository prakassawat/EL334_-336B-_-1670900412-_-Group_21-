void Buzzer_Init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    PORTA->PCR[1] = PORT_PCR_MUX(1);

    // Set Input
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

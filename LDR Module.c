void LDR_Init(void)
{
    // Enable Clock for PORTA
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    // D5 = PTA2 as GPIO
    PORTA->PCR[2] = PORT_PCR_MUX(1);

    // Set Input
    PTA->PDDR &= ~LDR_PIN;
}

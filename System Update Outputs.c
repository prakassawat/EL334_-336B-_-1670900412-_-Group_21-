void System_UpdateOutputs(void)
{
    // --------------------------------------
    // AUTO MODE
    // --------------------------------------

    if (system_mode == MODE_AUTO)
    {
        LED2_On();

        // D0 HIGH = Light
        // D0 LOW  = Dark

        if ((PTA->PDIR & LDR_PIN) != 0U)
        {
            LED1_On();
        }
        else
        {
            LED1_Off();
        }
    }

    // --------------------------------------
    // MANUAL MODE
    // --------------------------------------

    else if (system_mode == MODE_MANUAL)
    {
        LED2_Off();

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
        LED2_Off();

        if ((timer_running != 0U) &&
            (alarm_active == 0U))
        {
            LED1_On();
        }
        else
        {
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

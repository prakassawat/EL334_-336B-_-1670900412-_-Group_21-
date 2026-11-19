int main(void)
{
    // Initialize All Devices

    LED_Init();

    Switch_Init();

    SevenSeg_Init();

    Buzzer_Init();

    LDR_Init();

    PIT_Init();


    // Initialize System Variables

    countdown_seconds = TIMER_START_SECONDS;

    timer_running = 0U;
    timer_finished_event = 0U;
    alarm_active = 0U;

    manual_led1_state = 0U;

    // Start in AUTO MODE
    system_mode = MODE_AUTO;

    // Update Initial Outputs
    System_UpdateOutputs();


    // MAIN LOOP

    while (1)
    {
        // Refresh 7-Segment
        SevenSeg_Refresh();


        // ----------------------------------
        // Check Timer Finished
        // ----------------------------------

        if (timer_finished_event != 0U)
        {
            __disable_irq();

            timer_finished_event = 0U;

            __enable_irq();

            // Turn On Alarm When Timer Finished
            alarm_active = 1U;
        }


        // ----------------------------------
        // Check S1
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

                // Reset Manual LED State
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
        // Check S2
        // ----------------------------------

        if (sw2_event != 0U)
        {
            __disable_irq();

            sw2_event = 0U;

            __enable_irq();


            // S2 in MANUAL MODE

            if (system_mode == MODE_MANUAL)
            {
                // Toggle LED1
                manual_led1_state ^= 1U;
            }


            // S2 in TIMER MODE

            else if (system_mode == MODE_TIMER)
            {
                // Stop Alarm
                if (alarm_active != 0U)
                {
                    alarm_active = 0U;

                    Timer_Stop();

                    countdown_seconds = TIMER_START_SECONDS;
                }

                // Stop Timer
                else if (timer_running != 0U)
                {
                    Timer_Stop();

                    countdown_seconds = TIMER_START_SECONDS;
                }

                // Start Timer
                else
                {
                    // Start 30 Seconds Countdown
                    Timer_Start();
                }
            }
        }


        // ----------------------------------
        // Update LED and Buzzer
        // ----------------------------------

        System_UpdateOutputs();
    }
}

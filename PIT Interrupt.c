void PIT0_IRQHandler(void)
{
    // Clear Interrupt Flag
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

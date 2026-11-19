typedef enum
{
    MODE_AUTO = 0U,
    MODE_MANUAL,
    MODE_TIMER
} SystemMode;

// 7-Segment Code
const uint8_t SEGMENT_CODE[10] =
{
    0xC0,
    0xF9,
    0xA4,
    0xB0,
    0x99,
    0x92,
    0x82,
    0xF8,
    0x80,
    0x90
};

// Digit Selection
const uint8_t DIGIT_SELECT[4] =
{
    0x01,
    0x02,
    0x04,
    0x08
};

// Switch Events
volatile uint8_t sw1_event = 0U;
volatile uint8_t sw2_event = 0U;

// Timer Variables
volatile uint32_t countdown_seconds = TIMER_START_SECONDS;
volatile uint8_t timer_running = 0U;
volatile uint8_t timer_finished_event = 0U;

// Alarm
uint8_t alarm_active = 0U;

// Manual Mode
uint8_t manual_led1_state = 0U;

// System Mode
SystemMode system_mode = MODE_AUTO;

// Bus Clock
uint32_t pit_bus_clock_hz = 0U;


// FUNCTION PROTOTYPES

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

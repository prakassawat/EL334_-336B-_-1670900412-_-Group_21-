# ARM K64 Multi-Function Shield Project

## Project Overview

This project uses the ARM Cortex-M4 (NXP K64F) microcontroller with a Multi-Function Shield and external sensors.

The system integrates LED, Switch, 7-Segment Display, Buzzer, LDR, Hardware Timer, and Interrupt functionality.

## Hardware Components

- ARM K64F (FRDM-K64F)
- Multi-Function Shield
- LED1 and LED2
- Switch S1 and S2
- 4-Digit 7-Segment Display
- Buzzer
- LDR Module

## System Operating Modes

### 1. AUTO MODE
- Uses the LDR digital input to detect light conditions.
- Controls LED1 automatically.
- LED2 indicates AUTO MODE.

### 2. MANUAL MODE
- Uses Switch S2 to turn LED1 ON or OFF.
- Allows manual LED control.

### 3. TIMER MODE
- Uses Switch S2 to start a 30-second countdown.
- Displays the remaining time on the 7-Segment Display.
- Activates the Buzzer when the countdown reaches zero.

Switch S1 is used to change between the three operating modes.

## Pin Configuration

| Component | K64F Pin |
|---|---|
| LED1 | PTD1 |
| LED2 | PTD3 |
| Switch S1 | PTB3 |
| Switch S2 | PTB10 |
| 7-Segment Latch | PTB23 |
| 7-Segment Clock | PTC3 |
| 7-Segment Data | PTC12 |
| Buzzer | PTA1 |
| LDR Digital Output | PTA2 |

## Source Code Structure

| File | Description |
|---|---|
| 7-Segment Module.c | Controls the 7-Segment Display |
| Buzzer Module.c | Controls the Buzzer |
| LDR Module.c | Initializes the LDR input |
| LED Module.c | Controls LED1 and LED2 |
| LibraryandPIN.c | Defines libraries and hardware pins |
| Main Program.c | Main program and system control |
| PIT Hardware Timer.c | Configures and controls the PIT Timer |
| PIT Interrupt.c | Handles timer interrupts |
| Switch Module and PORTB Interrupt.c | Handles switches and PORTB interrupts |
| System Update Outputs.c | Updates LED and Buzzer outputs |
| VariableandFunction Prototypes.c | Defines system variables and function prototypes |

## Development Environment

- Keil MDK-Arm
- NXP K64F Microcontroller
- NXP MK64F12 Device Family Pack

## Main Features

- Automatic LED control using LDR
- Manual LED control using switches
- 30-second countdown timer
- 4-digit 7-Segment display
- Buzzer alarm
- GPIO and hardware timer interrupts

## Author

ARM K64 Mini Project

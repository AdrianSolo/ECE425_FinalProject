/*
 * @file main.c
 *
 * @brief Main source code for the ECE 425 Final Project
 *
 * This file contains the main entry point and function definitions for the Home Security System program.
 * It interfaces with the following:
 *	- EduBase Board LEDs (LED0 - LED3)
 *	- EduBase Board 16x2 Liquid Crystal Display (LCD)
 *  - EduBase Bord Buttons (SW2-SW5)
 *  - US-100 UltraSonic Sensor
 *  - EduBase Board Buzzer
 *
 * 
 * 
 *
 * @author Adrian Solorzano
 */

#include "TM4C123GH6PM.h"
#include "Buzzer.h"
#include "SysTick_Delay.h"
#include "EduBase_LCD.h"
#include "Timer_0A_Interrupt.h"
#include "GPIO.h"
#include "stdio.h"
#include "Security.h"
#include "UART1.h"

// Global state
static uint8_t system_armed = 0; // 0 = Disarmed, 1 = Armed
static uint8_t alert_active = 0; // 0 = No alert, 1 = Alert triggered

void Menu_Controller(uint8_t edubase_button_status);
void Display_Status(const char *message);

int main(void)
{
    // Initializes system peripherals
    SysTick_Delay_Init();       // Initialize SysTick timer for delays
    EduBase_LCD_Init();         // Initialize the 16x2 LCD on the EduBase board
    EduBase_LEDs_Init();        // Initialize the LEDs on the EduBase board
    EduBase_Button_Init();      // Configure buttons with interrupts
    Buzzer_Init();              // Initialize the buzzer
    UART1_Init();               // Initialize UART1 for US-100 sensor communication

    SysTick_Delay1ms(100);      // Allow peripherals to stabilize

    // Display the initial menu on the LCD
    Display_Main_Menu();

    EduBase_LEDs_Output(EDUBASE_LED_ALL_OFF); // Turn off all LEDs initially

    while (1)
    {
        // Get button status and handle menu interactions
        uint8_t edubase_button_status = Get_EduBase_Button_Status();
			
        Menu_Controller(edubase_button_status);

        SysTick_Delay1ms(10); // Add a small delay for stability
    }
}

// Used for selecting different options displayed on the LCD
void Menu_Controller(uint8_t button_status)
{
    static uint8_t last_button_status = 0; // Store the last button status

    // Only update if the button status has changed
    if (button_status != last_button_status) 
			{
        last_button_status = button_status; // Update the last button status

        switch (button_status)
        {
            case 0x08: // SW2 pressed
                if (!system_armed) {
                    system_armed = 1;             // Arm the system
                    Display_Status("System Armed"); // Display armed message
                    System_Armed();               // Start scanning immediately after arming
                } else {
                    Display_Status("Already Armed"); // Display already armed message
                }
                Display_Main_Menu(); // Return to the main menu
                break;

            case 0x04: // SW3 pressed
                if (system_armed) {
                    system_armed = 0;               // Disarm the system
                    Display_Status("System Disarmed"); // Display disarmed message
                } else {
                    Display_Status("Already Disarmed"); // Display already disarmed message
                }
                Display_Main_Menu(); // Return to the main menu
                break;

            case 0x01: // SW5 pressed
                Intruder_Alert(); // Trigger the intruder alert
                break;

            case 0x02: // SW4 pressed
                EduBase_LCD_Clear_Display(); // Clear the LCD
                Display_Main_Menu();         // Redisplay the main menu
                SysTick_Delay1ms(20);        // Short debounce delay
                break;

            default:
                break;
        }
    }
}

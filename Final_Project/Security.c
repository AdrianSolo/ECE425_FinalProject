/*
 * @file Security.c
 *
 * @brief Source code for the security system implementation.
 *
 * This file contains the primary logic for the security system, including:
 * - Monitoring the armed/disarmed state.
 * - Interfacing with the US-100 Ultrasonic Distance Sensor to detect intrusions.
 * - Triggering alerts through LEDs, the buzzer, and the LCD.
 *
 * The system continuously monitors for intrusions while armed and activates 
 * an alert if an object is detected within a predefined distance threshold.
 *
 * Key features:
 * - User interaction via buttons to arm/disarm the system.
 * - Visual and auditory response using EduBase Board peripherals.
 * - Real-time distance measurement using UART communication with the US-100 sensor.
 *
 * @note For more information regarding the US-100 Ultrasonic Sensor, refer to its datasheet.
 * Link: https://www.elecrow.com/download/US-100.pdf
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

// Global state variables
static uint8_t system_armed = 0; // 0 = Disarmed, 1 = Armed
static uint8_t alert_active = 0; // 0 = No alert, 1 = Alert triggered

// Constants for the buzzer state
extern const uint8_t BUZZER_OFF;
extern const uint8_t BUZZER_ON;

/**
 * @brief Monitors the system while it is armed and triggers an alert if necessary.
 *
 * Continuously checks the distance from the sensor. If the distance falls within 
 * the intrusion limit, it triggers the alert sequence. Returns to the main menu 
 * if the system is disarmed.
 */
void System_Armed(void)
{
    alert_active = 0;

    while (system_armed && !alert_active)
    {
        // Get the distance from the sensor
        uint16_t distance = Get_Distance();

        // Check if the object is within the threshold
        if (distance > 0 && distance <= 50) // Threshold of 50 cm
        {
            alert_active = 1;
            Intruder_Alert(); // Trigger the alert
        }

        // Add a delay to stabilize sensor readings
        SysTick_Delay1ms(100);
    }

    if (!system_armed)
    {
        Display_Main_Menu(); // Return to the main menu when disarmed
    }
}

#define READ_DISTANCE 0x55 // Command to read distance from US-100

/**
 * @brief Retrieves the distance measured by the US-100 sensor.
 *
 * Sends a command to the US-100 sensor and reads the response, which includes
 * the high and low bytes of the distance measurement.
 *
 * @return uint16_t The measured distance in millimeters.
 */
uint16_t Get_Distance(void)
{
    // Send the "read distance" command (0x55)
    UART1_Output_Character(READ_DISTANCE);

    // Receive the high and low bytes from the sensor
    uint8_t high_byte = UART1_Input_Character();
    uint8_t low_byte = UART1_Input_Character();

    // Combine the bytes to form the distance value
    uint16_t distance = (high_byte << 8) | low_byte;

    // Return the calculated distance
    return distance;
}

/**
 * @brief Displays the main menu on the LCD.
 *
 * Shows the options to arm or disarm the system on the EduBase LCD.
 */
void Display_Main_Menu(void)
{
    // Clear the LCD before displaying new content
    EduBase_LCD_Clear_Display();
    
    // Display "Arm System" on the first row
    EduBase_LCD_Set_Cursor(0, 0);
    EduBase_LCD_Display_String("Arm System");

    // Display "Disarm System" on the second row
    EduBase_LCD_Set_Cursor(0, 1);
    EduBase_LCD_Display_String("Disarm System");
}

/**
 * @brief Triggers the intruder alert sequence.
 *
 * Activates the LEDs, sounds the buzzer, and displays a warning message on the LCD.
 * Resets the system state after the alert sequence ends.
 */
void Intruder_Alert(void)
{
    // Display the alert message
    EduBase_LCD_Clear_Display();
    EduBase_LCD_Set_Cursor(0, 0);
    EduBase_LCD_Display_String("Intruder");
    EduBase_LCD_Set_Cursor(0, 1);
    EduBase_LCD_Display_String("Detected");
    SysTick_Delay1ms(3000); // Display message for 3 seconds

    // Flash LEDs and sound buzzer
    for (int i = 0; i < 10; i++) // Flash for 10 cycles
    {
        EduBase_LEDs_Output(EDUBASE_LED_ALL_ON); // Turn all LEDs on
        Play_Note(A4_NOTE, 50);                  // Sound alert tone
        SysTick_Delay1ms(250);

        EduBase_LEDs_Output(EDUBASE_LED_ALL_OFF); // Turn all LEDs off
        Play_Note(G4_NOTE, 50);                  // Sound alternate tone
        SysTick_Delay1ms(250);
    }

    // Reset alert state and return to main menu
    alert_active = 0;                     // Reset alert flag
    system_armed = 0;                     // Disarm the system
    EduBase_LEDs_Output(EDUBASE_LED_ALL_OFF); // Turn off LEDs
    Buzzer_Output(BUZZER_OFF);            // Turn off buzzer
    Display_Main_Menu();                  // Return to the main menu
}

/**
 * @brief Displays a custom status message on the LCD.
 *
 * Clears the LCD and displays the provided message for 3 seconds.
 *
 * @param message A pointer to the string containing the message to display.
 */
void Display_Status(const char *message)
{
    EduBase_LCD_Clear_Display();
    EduBase_LCD_Set_Cursor(0, 0);
    EduBase_LCD_Display_String(message);
    SysTick_Delay1ms(3000); // Delay to show the message for 3 seconds
}

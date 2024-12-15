/**
 * @file Security.h
 *
 * @brief Header file for the Security driver.
 *
 * This file contains the function definitions for the Security driver, 
 * which manages the system's armed state and intrusion detection.
 *
 * @author Adrian Solorzano 
 */

#include "TM4C123GH6PM.h"
#include "stdio.h"
#include "UART1.h"

/**
 * @brief Activates the system's armed state and continuously checks for intrusions.
 *
 * This function activates the security system. If an intrusion is detected 
 * based on the sensor's input, an alert is triggered.
 *
 * @param None
 * @return None
 */
void System_Armed(void);

/**
 * @brief Triggers the alert mechanism during an intrusion.
 *
 * This function activates visual and audio signals to alert the user of an intrusion.
 *
 * @param None
 * @return None
 */
void Intruder_Alert(void);

/**
 * @brief Displays the main menu on the output interface.
 *
 * This function clears the current interface and displays the main menu options.
 *
 * @param None
 * @return None
 */
void Display_Main_Menu(void);

/**
 * @brief Displays a custom status message on the output interface.
 *
 * This function displays a given status message, which can be used to show 
 * the system state or specific alerts.
 *
 * @param message A string containing the message to display.
 * @return None
 */
void Display_Status(const char *message);

/**
 * @brief Retrieves the current distance from the ultrasonic sensor.
 *
 * This function communicates with the US-100 sensor to get the current distance 
 * in millimeters.
 *
 * @param None
 * @return uint16_t The measured distance in millimeters.
 */
uint16_t Get_Distance(void);
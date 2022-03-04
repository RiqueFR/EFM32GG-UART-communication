#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "em_device.h"

#include "clock_efm32gg_ext.h"
#include "button.h"
#include "led.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"

#define Button_Active(button) !((button & Button_Read()))
#define SERIAL_UPDATE 100    // milliseconds

// ADC Channel 6
/*************************************************************************/ /**
                                                                             * @brief  Sys Tick Handler
                                                                             */
const int TickDivisor = 1000;                                               // milliseconds
const int TickMax = 10000;                                               // milliseconds
static int counter = 0;

void SysTick_Handler(void)
{
    counter++;
	if(counter > TickMax) counter = 0;
}

void clean_buffer() {
	for(int i = 0; i < 10; i++) {
		UART_SendChar('\n');
		UART_SendChar('\r');
	}
}

void my_concat(char* result, int val, const char* frase, int base) {
	char aux[10];
	aux[0] = '\0';
	strcat(result, frase);
	itoa(val, aux, base);
	if(base == 16)
		strcat(result, "0x"); 
	else if(base == 2)
		strcat(result, "0b"); 
	strcat(result, aux); 
	strcat(result, "\n\r"); 
}

void print_info(int ADC, int button1, int button2, int LED_status, int pwm_value) {
	char info[500];
	info[0] = '\0';
	char aux[10];
	aux[0] = '\0';
	my_concat(info, ADC, "Valor ADC: ", 10);
	my_concat(info, button1, "Valor PB0: ", 10);
	my_concat(info, button2, "Valor PB1: ", 10);
	my_concat(info, LED_status, "Valor LED: ", 10);
	my_concat(info, pwm_value, "Valor PWM: ", 16);
	clean_buffer();
	UART_SendString(info);
}

/*****************************************************************************
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {
    ClockConfiguration_t clockconf;

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

#if 1
    ClockGetConfiguration(&clockconf);
#endif

    // Configure LEDs
    LED_Init(LED1 | LED2);

    // Configure SysTick
    SysTick_Config(SystemCoreClock/TickDivisor);

    // Configure Buttons
    Button_Init(BUTTON1 | BUTTON2);

    // Configure PWM to LED0
    PWM_Init(TIMER3, PWM_LOC1, PWM_PARAMS_ENABLECHANNEL2|PWM_PARAMS_ENABLEPIN2);
    PWM_Write(TIMER3, 2, 0);

    // Configure ADC
    ADC_Init(5000);
    ADC_ConfigChannel(ADC_CH0, 0);

    // Configure UART
    UART_Init();

	// Initialize variables
	char buffer[201];
	char command[201];
	int command_size = 0;
	int buffer_pos = 0;
	char ch;
	int pwm_value = 0;
	int adc_result = 0;
	int buttons[2] = {0};
	int LED_status = 0;

    while (1)
    {
        ch = UART_GetCharNoWait();

		// Create the command string
        if (ch != 0) {
			if(ch == '\n' || ch == '\r') {
				strcpy(command, buffer);
				command[buffer_pos] = '\0';
				buffer[buffer_pos++] = '\n';
				command_size = buffer_pos;
				buffer[buffer_pos++] = '\r';
				buffer[buffer_pos++] = '\0';
				UART_SendString(buffer);
				buffer_pos = 0;
				continue;
			}
            buffer[buffer_pos] = ch;
            buffer_pos++;
        }

		// Check buttons
        if (Button_Active(BUTTON1))
			buttons[0] = 1;
        if (Button_Active(BUTTON2))
			buttons[1] = 1;

		// Check PWM command
        if (strlen(command) > 4 && command[0] == 'P' &&
		  command[1] == 'W' && command[2] == 'M') {
            command[command_size] = '\0';
            pwm_value = atoi(command + 4);
            PWM_Write(TIMER3, 2, pwm_value);
        }

		// Read ADC
		adc_result = ADC_Read(ADC_CH0);

		// Check LED command and control LED2
		if (strcmp(command, "LED ON") == 0) {
			LED_On(LED2);
			LED_status = 1;
		} else if (strcmp(command, "LED OFF") == 0) {
			LED_Off(LED2);
			LED_status = 0;
		}

		// Only send info through serial every SERIAL_UPDATE ms
		if(counter - SERIAL_UPDATE > 0) {
			print_info(adc_result, buttons[0], buttons[1], LED_status, pwm_value);
			counter = 0;
		}

		// refresh variables
		buttons[0] = 0; buttons[1] = 0;
		command[0] = '\0';
		command_size = 0;
	}
}

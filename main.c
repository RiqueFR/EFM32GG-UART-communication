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

// ADC Channel 6
/*************************************************************************/ /**
                                                                             * @brief  Sys Tick Handler
                                                                             */
const int TickDivisor = 1000;                                               // milliseconds
uint16_t ms_counter = 0;

void SysTick_Handler(void)
{
    static int counter = 0;
    if (counter == 0)
    {
        counter = TickDivisor;
        // Process every second
    }
    counter--;
}


void imprime_mensagem(char *mensagem)
{
    int i = 0;
    while (mensagem[i] != '\0')
    {
        UART_SendChar(mensagem[i]);
        i++;
    }
}

void limpar_mensagem(char *mensagem)
{
    int i = 0;
    while (mensagem[i] != '\0')
    {
        mensagem[i] = '\0';
        i++;
    }
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

    // Configure ADC
    ADC_Init(5000);
    ADC_ConfigChannel(ADC_CH0, 0);

    // Configure UART
    UART_Init();

    int cntchar = 0;
    char ch = 0;

    int status = 0;
    char *pwm_led1 = "LED1 PWM";
    char *liga_led2 = "LED2 ON";
    char *desliga_led2 = "LED2 OFF";

    unsigned pwm_value = 0;

    int adc_result = 0;
    char adc_result_str[10];
    char input[100];
    PWM_Write(TIMER3, 2, 0);

    while (1)
    {
        // verifica se o botao PB1 foi pressionado e liga tudo

        ch = UART_GetChar();
        //ch = UART_GetCharNoWait();

        if (ch != 0 && ch != '\n' && ch != '\r')
        {
            input[cntchar] = ch;
            cntchar++;
        }
        else if (input[5] == 'P' && input[6] == 'W' && input[7] == 'M')
        {
            input[cntchar] = '\0';
            cntchar = 0;
            pwm_value = atoi(input + 8);
            PWM_Write(TIMER3, 2, pwm_value);
        }
        else
        {
            input[cntchar] = '\0';
            cntchar = 0;

            char adc_string[20] = "ADC VALUE: ";
            adc_result = ADC_Read(ADC_CH0);

            itoa(adc_result, adc_result_str, 10);
            strcat(adc_string, adc_result_str);
            strcat(adc_string, "\n\r");
            imprime_mensagem(adc_string);

            if (strcmp(input, liga_led2) == 0)
                LED_On(LED2);

            else if (strcmp(input, desliga_led2) == 0)
                LED_Off(LED2);

            else if (Button_Active(BUTTON1))
                imprime_mensagem("PB0 pressionado\n\r");

            else if (Button_Active(BUTTON2))
                imprime_mensagem("PB1 pressionado\n\r");
        }
    }
}

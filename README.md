## Using all EFM32GG knowledge to create a communication UART program
 
<p>This code uses all previous EFM32GG knowledge and code to create a communication using UART.
You can send commands to turn LED1 on or off and send PWM value to control LED0 brightness.
You will receive a info and see if PB0 and PB1 are pressed and the ADC value for a potentiometer.
As always, all external libraries are from hans-jorg, and you can check his GitHub for more information.</p>
 
## How it works

<p>To use the code, connect a potentiometer to the pin PD0 and flash the code to the board.</p>
<p>When connected to the Serial Communication, the board will send the following information, with its respective values:</p>

    ADC: <value>
    PB0: <value>
    PB1: <value>
    LED: <value>
    PWM: <value>

<p>To change these values, you need eater interact with then, or send a command through serial communication. For example, to change PB0 and PB1 values, simply press them on the board, same to the ADC value, change the potentiometer position. To change the LED and PWM values, use the respective commands:</p>

    LED ON
    LED OFF

<p>To control the LED1 on or off, and:</p>

    PWM <int_value>

<p>To change the PWM value and the LED0 brightness.</p>
 
## More information
 
[hans-jorg EFM32GG Projects](https://github.com/hans-jorg/efm32gg-stk3700-gcc-cmsis)


# Converting-Deep-Freezer-into-Refrigerator---Pic18f4520
Created a switch using Servo motor to turn on / off the deep freezer. Servo controlled by PWM pulses. 

The on off switch is controlled with the help of servo motor. 
Pin configuration:
RA0 : LM35 temperature sensor connection
RC0 : LED ON for temperature between 1-4 deg Celcius.
RC2 : PWM output on PIC. Servo controlled with PWM.
RD0-RD7 : Data pins of LCD.
RE0 : Rs pin of LCD
RE1 : rw pin of LCD
RE2 : en pin of LCD

# calculations for registers:
Internal oscillator selected with frequency 8MHz. declared in OSCCON register.
For using PWM, CCP1CON register is used. CCP1CON uses Timer2. 
1) Fosc = 8 MHz (internal Oscillator)
   Therefore, OSCCON = 0x72
2) Fpwm = 10KHz
3) PR2 = 199
   
Alternatively, time delays can also be used to control the servo motor. 
0-180deg servo motor selected which moves back and forth from 0deg to 180deg
0deg -> 3% duty cycle -> 1.5 milli sec
90deg -> 12% duty cycle -> 2 milli sec

# for 3% duty cycle :
CCPRxL : CCPxCON <5:4> = (200 * 3)/100 = 6(dec) = 0b0000000110
hence, CCPRxL = 1 -> 8bits MSB
CCPxCON = 0x2C

# for 12% duty cycle :
CCPRxL : CCPxCON <5:4> = (200 * 12)/100 = 24(dec) = 0b0000011000
hence, CCPRxL = 0b00000110 -> 24(dec) -> 8bits MSB
CCPxCON = 0x0C -> 2 bits LSB and PWM values.


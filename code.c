/*
Code to convert a deep freezer into refrigerator. The on off switch is controlled with the help of servo motor. 
pin configuration:
RA0 : LM35 temperature sensor connection
RC0 : LED ON for temperature between 1-4 deg Celcius.
RC2 : PWM output on PIC. Servo controlled with PWM.
RD0-RD7 : Data pins of LCD.
RE0 : Rs pin of LCD
RE1 : rw pin of LCD
RE2 : en pin of LCD
Internal oscillator selected with frequency 8MHz. declared in OSCCON register.
For using PWM, CCP1CON register is used. CCP1CON uses Timer2. 
Alternatively, time delays can also be used to control the servo motor. 
0-180deg servo motor selected which moves back and forth from 0deg to 180deg
0deg -> 3% duty cycle -> 1.5 milli sec
90deg -> 12% duty cycle -> 2 milli sec
 
*/
#include<p18f4520.h>

#define ldata PORTD
#define rs PORTEbits.RE0
#define rw PORTEbits.RE1
#define en PORTEbits.RE2

#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config DEBUG = OFF
#pragma config PWRT = OFF

void configureA2D(void);
void startConversion(void);
unsigned int getADCResult(void);
void showTemp(unsigned int adc_val);
void checkTemp(unsigned int adc_val);
void servoOn_right(void);
void servoOn_left(void);
void lcdcmd(unsigned char value);
void lcddata(unsigned char value);
void lcddelay(unsigned int itime);

void main()
{
	unsigned int adc_val, voltage;
	
	//configure analog to digital module
	ADCON1 = 0x0F;
	TRISD = 0x00;
	TRISE = 0x00;
	TRISC = 0x00;
	OSCCON = 0x72; //internal 8MHz oscillator
	PR2 = 199;	//loading value into PR2 register
	//T2CON = 0x01;	//Prescalar value set to 4 . no prescalar chosen
	TRISAbits.RA0 = 1;
	while(1)
	{
	configureA2D();
	startConversion();
	adc_val = getADCResult();
	adc_val = (long)adc_val * 500;
	adc_val = (unsigned int)adc_val/1023; //temperature
	
	lcdcmd(0x38);	//function 8 bit : set 2 line display 5x7 matrix
	lcddelay(1);	
	lcdcmd(0x0E);	//clear display
	lcddelay(1);
	lcdcmd(0x01);	//Display on, cursor on, blinker on
	lcddelay(1);
	lcdcmd(0x06);	//Entry mode
	lcddelay(1);
	lcdcmd(0x80);	//Display on cursor on
	lcddelay(1);
	lcddata('T');
	lcddelay(1);
	lcddata('E');
	lcddelay(1);
	lcddata('M');
	lcddelay(1);
	lcddata('P');
	lcddelay(1);
	PORTCbits.RC0 = 0;
	
	checkTemp(adc_val);
	showTemp(adc_val);
	}
}
void checkTemp(unsigned int adc_val)
{
	if (adc_val > 0 && adc_val < 5)
	{
		//turn off the refigerator
		PORTCbits.RC0 = 1;
		servoOn_left(); //left
	}
	else{
		PORTCbits.RC0 = 0;
		servoOn_right();
	}
}
void configureA2D(void)
{
	ADCON1 = 0X0E;		//configure analog pins and digital I/O
	ADCON0 = 0X00;		//Select A/D input channel. Here channel 0
	ADCON2 = 0b10001110;//acquisition time and conversion clock
	ADCON0bits.ADON = 1;//turn on A/D module
}
void startConversion(void)
{
	ADCON0bits.GO = 1; //starts conversion
}
unsigned int  getADCResult(void)
{
	unsigned int ADC_RESULT;
	while(ADCON0bits.DONE);
		ADC_RESULT = ADRESL;
		ADC_RESULT |= ((unsigned int)ADRESH) << 8;
	return ADC_RESULT;
}
void lcddata(unsigned char value)
{
	ldata = value;
	rs = 1; // selection of data register of lcd
	rw = 0;
	en = 1;
	lcddelay(1);
	en = 0;
}
void lcdcmd(unsigned char value)
{
	ldata = value;
	rs = 0;
	rw = 0;
	en = 1;
	lcddelay(1);
	en=0;
}
void lcddelay(unsigned int itime)
{
	unsigned int i, j; 
	for(i=0;i<itime;i++);
	for(j=0;j<150;j++);
}

void showTemp(unsigned int adc_val)
{
	unsigned int i = 2, j;
	unsigned char arr[3];
	unsigned int temperature;
	temperature = adc_val;
	while(temperature > 0 && i >= 0)
	{
		arr[i] = (temperature % 10) + 0x30; //converting to ASCII
		temperature = temperature / 10;
		i--;

	}
	lcdcmd(0xC0);
	lcddelay(1);
	for(j=0; j<3; j++)
	{
		lcddata(arr[j]);
	}
}
void servoOn_right(void)
{
	CCPR1L = 24;	//12% duty cycle
	CCP1CON = 0x0C;
	lcddelay(1);
	TMR2 = 0x00;	//timer is first off
	PIR1bits.TMR2IF = 0;	
	T2CONbits.TMR2ON = 1;	//timer on. starts counting
	while(PIR1bits.TMR2IF == 0); //check for interrupt flag
}
void servoOn_left(void)
{
	CCPR1L = 1;			//3% duty cycle
	CCP1CON = 0x2C;
	lcddelay(1);
	TMR2 = 0x00;
	PIR1bits.TMR2IF = 0;
	T2CONbits.TMR2ON = 1;
	while(PIR1bits.TMR2IF == 0); 
}
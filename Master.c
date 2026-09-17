/* This program writes different data to a LCD display device. Characters, strings,
unique characters and numbers are being displayed.The LCD is also activated using the 4 bit
display method and the R/W line is permanently connected to ground as only writing occur.
The program for the LCD was changed to accommodate PortB as the Proteus design shows.
*/

#include <avr/io.h>


#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#define Displaytime 0	//Bit 0 in All_Flags Indicate that the RTC menu setting is entered
#define Date_Time_MenuFlag 1//Bit 1 in All_Flags Indicate Date and time menu entered
#define KeyPressedFlag 2	//Bit 2 in All_Flags Indicate a key was pressed

volatile unsigned int  All_Flags = 0;
volatile unsigned char Keyrec;
volatile unsigned char Buffer;
volatile unsigned char Hour24, year,month,day,hour,min,sec, Minutes,Seconds;
volatile unsigned char Date_Time[13];
volatile unsigned char Counter=0;
unsigned char counter_data=0;
volatile unsigned int Recieved[4];
#define done_status 5


void lcdCommand( unsigned char cmnd ){
PORTB = (PORTB & 0xF0) | (cmnd >> 4);	//Move upper command byte to Data lines of LCD
PORTB &= ~ (1<<5);					//make RS pin Low
PORTB |= (1<<4);					//Make enable pin high
_delay_us(1);
PORTB &= ~ (1<<4);					//Make enable pin low
_delay_us(100);
PORTB = (PORTB & 0xF0) | (cmnd & 0x0F); //Move lower command byte to Data lines of LCD
PORTB |= (1<<4);					//Make enable pin high
_delay_us(1);
PORTB &= ~ (1<<4);					//Make enable pin low
_delay_us(100);
}

void lcdData( unsigned char data )
{
	PORTB = (PORTB & 0xF0) | (data >>4);  //Write upper data nibble to lcd
	PORTB |= (1<<5);					//RS line high
	PORTB |= (1<<4);				//Make En pin high
	_delay_us(1);
	PORTB &= ~ (1<<4);				//Make En pin low
	PORTB = (PORTB & 0xF0) | (data & 0x0F);//Write lower data nibble to lcd
	PORTB |= (1<<4);				//Make En pin high
	_delay_us(1);
	PORTB &= ~ (1<<4);				//Make En pin low
	_delay_us(100);
}

void lcd_init(){
	DDRB = 0b00111111;				//Port B pins 0 to 5 used for LCD-outputs
	PORTB &=~(1<<4);				//Make En pin low
	_delay_us(2000);				//wait 2 ms
	lcdCommand(0x33);
	lcdCommand(0x32);
	lcdCommand(0x28);
	lcdCommand(0x0C);
	lcdCommand(0x01);
	_delay_us(2000);
}

void lcd_gotoxy(unsigned char x, unsigned char y)
{
	unsigned char firstCharAdr[] = {0x80, 0xC0, 0x94, 0xD4};
	lcdCommand(firstCharAdr[y-1] + x - 1);
	_delay_us(100);
}

void lcd_print( char * str )
{
	unsigned char i = 0 ;
	while(str[i]!=0)		//looking for nul character
	{
		lcdData(str[i]);
		i++ ;
	}
}
//*******************************************************
void lcd_clear(void)
{
lcd_gotoxy(1,1);
lcd_print("                ");
lcd_gotoxy(1,2);
lcd_print("                ");
}
//*******************************************************
void lcd_variable_print(char Digits, unsigned int Var)
{
	unsigned char Cntr=0, SizeOffset=0, Temp=0;
	unsigned int Val=0;
	char DispArr[5];

	for (Cntr = 5; Cntr > 0; Cntr--){
		Val = Var%10;
		Var = Var/10;
		Temp = (char)Val;
		DispArr[Cntr-1] = (Temp) + 0x30;   // Determine BCD value and convert to ASCII
	}
	if(Digits == 1){SizeOffset = 4;}					//1 character
	if(Digits == 2){SizeOffset = 3;}	   					//2 characters
	if(Digits == 3){SizeOffset = 2;}					    //3 characters
	if(Digits == 4){SizeOffset = 1;}			 			//4 characters
	if(Digits == 5){SizeOffset = 0;}						//5 characters
	for (Cntr = SizeOffset; Cntr < 5; Cntr++){
		lcdData(DispArr[Cntr]);   // Write ASCII data to the display
		_delay_ms(5);                    // Delay 5 ms
	}
}

//********I2C functions*************************************
void i2c_write(unsigned char data)
{
  TWDR = data ;
  TWCR = (1<< TWINT)|(1<<TWEN);
  while ((TWCR & (1 <<TWINT)) == 0);
}

//**********************************************************
void i2c_start(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while ((TWCR & (1 << TWINT)) == 0);
}

//**********************************************************

void i2c_stop()
{
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

//**********************************************************

void i2c_init(void)
{
  TWSR=0x00;	//set prescaler bits  to zero
  TWBR=152;	//SCL frequency is 50K for XTAL = 16M
  TWCR=0x02;	//enable the TWI module
}

//**********************************************************
unsigned char i2c_read(unsigned char ackVal)
{
	TWCR |= (1<<TWINT)|(1<<TWEN);
	//TWCR = (1<< TWINT)|(1<<TWEN)|(ackVal<<TWEA);
	if(ackVal==1)//Ack
	{
		TWCR |= (1<<TWEA);  //Ack
	}
	else  //nack
	{
		TWCR &= ~(1<<TWEA);  //nack
	}
	while ((TWCR & (1 <<TWINT))==0);
	return TWDR;
}
//************************************************************
void Sqw_init(void)
{
	i2c_start();		//transmit START condition
	i2c_write(0xD0);	//address DS1307 for write SLA + W(0)
	i2c_write(0x07);	//set register pointer to 7
	i2c_write(0x10);	//set value of location 7 to 0, Square wave disable
	i2c_stop();			//transmit STOP condition
}

//************************************************************

void rtc_setTime(unsigned char h,unsigned char m,unsigned char s)
{//See figure 4 in, Page 12 Dallas Maxim data sheet
	i2c_start();		//transmit START condition
	i2c_write(0xD0);	//address DS1307 for write
	i2c_write(0x00);	//set register pointer to 0
	i2c_write(s);		//set seconds
	i2c_write(m);		//set minutes
	i2c_write(h);		//set hour , 24hr format
	i2c_stop();		//transmit STOP condition
}

//************************************************************


volatile unsigned char High, Low;
void lcd_split(unsigned char BCD_Time)
{
	High = BCD_Time>>4;
	Low = BCD_Time & 0x0F;;
}
//************************************************************

void rtc_getTime(volatile unsigned char *h,volatile unsigned char *m,volatile unsigned char *s)
{//See figure 5 in, Page 12 Dallas Maxim data sheet
	i2c_start();		//transmit START condition
	i2c_write(0xD0);		//address DS1307 for write
	i2c_write(0);		//set register pointer to 0, first register in memory
	i2c_stop();		//transmit STOP condition

	i2c_start();		//transmit START condition
	i2c_write(0xD1);		//address DS1307 for read
	*s = i2c_read(1);	//read second, return ACK
	*m = i2c_read(1);	//read minute, return ACK
	*h = i2c_read(0);	//read hour, return NACK
	i2c_stop();		//transmit STOP condition
}


//************************************************************
ISR(INT0_vect)			//ISR for external interrupt 0, keypad
{
	unsigned char KeyArray[]={2,14,0,15,7,8,9,4,5,6,13,12,11,10,3,1};

	Buffer = PIND;  					//Read and mask data from PIC
	Buffer = Buffer>>4;
	Keyrec = KeyArray[Buffer];
	All_Flags |= (1<<KeyPressedFlag);		//Set flag to indicate that a key was pressed
	// to enable display in the main section
}
void usart_set()
{
	UCSR0B=(1<<RXEN0)|(1<<RXCIE0);
	UCSR0C=(1<<UCSZ00)|(1<<UCSZ01);
	UBRR0L=103;

}

ISR(USART_RX_vect)
{
	Recieved[counter_data]=UDR0;
	counter_data++;
	if(counter_data==4)
	{
		counter_data=0;
		All_Flags|=(1<<done_status);
	}

}
volatile unsigned int coil[16]={0000,1,10,11,100,101,110,111,1000,1001,1010,1011,1100,1101,1110,1111};
int main(void)
{
		lcd_init();
		usart_set();
		DDRD &= ~(1<<2);	//PD2 is Int 0 keypad input
		PORTD |= (1<<2);	//pull-up activated, PD2 is Int 0 input, input from start
		DDRD &= 0x0F;		//upper nibble inputs
		DDRD&=~(1<<0);
		PORTD |= 0xF0;		//enable pull up resistors on upper nibble of PortD
		EICRA = 0x02;		//p337 make INT0 falling edge triggered
		EIMSK |= (1<<INT0);	//p336 enable external interrupt 0
		i2c_init();
		Sqw_init();
		_delay_ms(50);

		sei ();			//enable interrupts

		while(1)
		{

			if((All_Flags & (1<<Displaytime)) == 0)// at startup or when done
			{
			rtc_getTime(&hour,&min,&sec);//See figure 5 in, Page 12 Dallas Maxim data sheet
			lcd_gotoxy(6,1);					//top line
			lcd_split(hour);
			lcd_variable_print(1,(unsigned int)High);
			lcd_variable_print(1,(unsigned int)Low);
			lcdData(0x3A);
			lcd_split(min);
			lcd_variable_print(1,(unsigned int)High);
			lcd_variable_print(1,(unsigned int)Low);
			lcdData(0x3A);
			lcd_split(sec);
			lcd_variable_print(1,(unsigned int)High);
			lcd_variable_print(1,(unsigned int)Low);



			if(All_Flags&(1<<done_status))
			{
				lcd_gotoxy(3,2);
				lcd_variable_print(2,(unsigned int)Recieved[1]);
				volatile unsigned int water= Recieved[2]|Recieved[3]<<8;
				lcd_gotoxy(7,2);
				lcd_variable_print(4,(unsigned int)water);
				lcd_gotoxy(13,2);
				unsigned char num=Recieved[0];
				lcd_variable_print(4,(unsigned int)coil[num]);
										lcd_gotoxy(1,1);
										lcd_print("Time=");
								 		lcd_gotoxy(1,2);
								 		lcd_print("T=");
								 		lcd_gotoxy(5,2);
								 		lcd_print("W=");
								 		lcd_gotoxy(11,2);
								 		lcd_print("S=");
				All_Flags&=~(1<<done_status);

			}
			}


	if(All_Flags & (1<<KeyPressedFlag))		//Key was pressed (KeyPressedFlag=1) and must be displayed
	{

	if(Keyrec == 14)		//* Indicate menu for RTC must be started
	{
	All_Flags |= (1<<Displaytime); //Set flag-Time will not be displayed
	lcd_clear();
	lcd_gotoxy(1,1);		//Top line
	lcd_print("Hour 00 to 23=");	//string print
	All_Flags |= (1<<Date_Time_MenuFlag); //This shows that hour high must be loaded
	All_Flags &= ~(1<<KeyPressedFlag);	//reset KeyPressedFlag to 0
	Counter++;
	}

		if ((All_Flags & (1<<KeyPressedFlag)) && (All_Flags & (1<<Date_Time_MenuFlag)))
		{
			lcd_variable_print(1,Keyrec);		//Print HourHigh
			Date_Time[Counter]=Keyrec;			//store received value
			All_Flags &= ~(1<<KeyPressedFlag);	//reset KeyPressedFlag to 0
		switch(Counter)
			{
				case 1: 	//Store HourHigh in Date_Time[1]
							Counter++;
							break;
				case 2: 	//Store HourLow in Date_Time[2]
							Counter++;
							_delay_ms(100);
							lcd_clear();
							lcd_gotoxy(1,1);		//Top line
							lcd_print("Min 00 to 59=");	//string print
							break;
				case 3: 	//Store MinHigh in Date_Time[3]
							Counter++;
							break;
				case 4: 	//Store MinLow in Date_Time[4]
							Counter++;
							_delay_ms(100);			//wait1 sec
							lcd_clear();
							lcd_gotoxy(1,1);		//Top line
							lcd_print("Sec 00 to 59=");	//string print
							break;
				case 5: 	//Store SecHigh in Date_Time[5]
							Counter++;
							break;
				case 6: 	//Store SecLow in Date_Time[6]

							Counter=0;
							lcd_clear();
							//Combine Time values and send to RTC
							Hour24 = (Date_Time[1]<<4)|(Date_Time[2]);		//HH|HL
							Minutes = (Date_Time[3]<<4)|(Date_Time[4]);		//MH|ML
							Seconds = (Date_Time[5]<<4)|(Date_Time[6]);		//SH|SL


							rtc_setTime(Hour24,Minutes,Seconds);
							All_Flags &= ~(1<<Displaytime); //clear flag-Time will be displayed
							All_Flags &= ~(1<<Date_Time_MenuFlag);//Clear menu flag
							break;
				default:	break;
     		}
		}
		}//End of if function


	}
	 return 0;
}

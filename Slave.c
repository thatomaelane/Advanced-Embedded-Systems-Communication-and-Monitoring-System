#include <avr/io.h>


#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

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
void usart_set()
{
	UCSR0B=(1<<TXEN0);
	UCSR0C=(1<<UCSZ00)|(1<<UCSZ01);
	UBRR0L=103;

}
void usart_transmit(unsigned char Data)
{
	while((UCSR0A&(1<<UDRE0))==0);
	UDR0=Data;
}
unsigned char uart_send[4];
volatile unsigned int coil[16]={0000,1,10,11,100,101,110,111,1000,1001,1010,1011,1100,1101,1110,1111};
unsigned int num=0;
int main(void)
{
   lcd_init();
   usart_set();
   DDRD=0x00;
   PORTD|=(0xF0);
   DDRC=0b11111100;

   lcd_gotoxy(1,1);
   lcd_print("T=    C");
   lcd_gotoxy(6,1);
   lcdData(0xDF);
   lcd_gotoxy(1,2);
   lcd_print("W=    L");

   lcd_gotoxy(10,2);
   lcd_print("S=");
   ADCSRA = 0x87;
   sei();
    while (1)
    {

		PORTC=(PIND&(0xF0))>>2;
		num=PIND>>4;
		lcd_gotoxy(12,2);
		lcd_variable_print(4,coil[num]);
		uart_send[0]=num;


	   for(int i=0;i<2;i++)
	   {
		   ADMUX= 0x40;
		   ADCSRA|=(1<<ADSC);
		   while((ADCSRA&(1<<ADIF))==0);
		   ADCSRA |= (1<<ADIF);
		   _delay_ms(10);
	   }
	    volatile unsigned char Tempa=ADC*0.4901960784;
		lcd_gotoxy(3,1);
		lcd_variable_print(3,Tempa);
		uart_send[1]=Tempa;


		for(int i=0;i<2;i++)
		{
			ADMUX= 0x41;
			ADCSRA|=(1<<ADSC);
			while((ADCSRA&(1<<ADIF))==0);
			ADCSRA |= (1<<ADIF);
		}
		volatile unsigned long Water=ADC*2.93255132;
		lcd_gotoxy(3,2);
		lcd_variable_print(4,Water);
		uart_send[2]=Water;
		uart_send[3]=Water>>8;
		for(int i=0; i<4; i++)
		{
			usart_transmit(uart_send[i]);
		}





    }
	return 0;
}

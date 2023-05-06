/*
 * GccApplication5.c
 *
 * Created: 5/4/2023 12:31:02 PM
 * Author : User
 */ 
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#define F_CPU 16000000UL
#define LCD_Dir  DDRB			/* Define LCD data port direction */
#define LCD_Port PORTB			/* Define LCD data port */
#define RS PB0				/* Define Register Select pin */
#define EN PB1 				/* Define Enable signal pin */
int l =0;
int x=0;
int* position;
int* up_or_down;//1 =move down 0=move up
int *stop;

void LCD_Command( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0); /* sending upper nibble */
	LCD_Port &= ~ (1<<RS);		/* RS=0, command reg. */
	LCD_Port |= (1<<EN);		/* Enable pulse */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_Char( unsigned char data )
{
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0); /* sending upper nibble */
	LCD_Port |= (1<<RS);		/* RS=1, data reg. */
	LCD_Port|= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (data << 4); /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Dir = 0xFF;			/* Make LCD port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	
	LCD_Command(0x02);		/* send for 4 bit initialization of LCD  */
	LCD_Command(0x28);              /* 2 line, 5*7 matrix in 4-bit mode */
	LCD_Command(0x0c);              /* Display on cursor off*/
	LCD_Command(0x06);              /* Increment cursor (shift cursor to right)*/
	LCD_Command(0x01);              /* Clear display screen*/
	_delay_ms(2);
}

void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* Clear display */
	_delay_ms(2);
	LCD_Command (0x80);		/* Cursor at home position */
}

void step_up(int x,int* y)
{
	int t=1500;
	for (int i=0;i<x ;i++)
	{
		if (*stop==1)
		{
			LCD_Clear();
			LCD_String("warning run");
		PORTA=0b00001111;
			break;
		}
		 else if (*stop==0)
		 {
		PORTA=0x01;
		_delay_ms(t);
		PORTA=0x02;
		_delay_ms(t);
		PORTA=0x04;
		_delay_ms(t);
		PORTA=0x08;
		_delay_ms(t);
		*y=*y+1;
		 }
		 if (*position<15)
		 {
			 LCD_Clear();
			 LCD_String("first floor");
		 }
		 else if (*position==30)
		 {
			 LCD_Clear();
			 LCD_String("third floor");
		 }
		 else
		 {
			 LCD_Clear();
			 LCD_String("second floor");
		 }
	}
}

void step_down(int x,int* y)
{
	int t=1500;
	for (int i=0;i<x ;i++)
	{
		if (*stop==1)
		{
			LCD_Clear();
			LCD_String("warning run");
				PORTA=0b00001111;
			break;
		}
	    else if (*stop==0)
		{
		PORTA=0x08;
		_delay_ms(t);
		PORTA=0x04;
		_delay_ms(t);
		PORTA=0x02;
		_delay_ms(t);
		PORTA=0x01;
		_delay_ms(t);
		*y=*y-1;
		 }
		 if (*position>15)
		 {
			 LCD_Clear();
			 LCD_String("third floor");
		 }
		 else if (*position==0)
		 {
			 LCD_Clear();
			 LCD_String("first floor");
		 }
		 else
		 {
			 LCD_Clear();
			 LCD_String("second floor");
		 }
	}
}

ISR(INT0_vect)
{
	PORTA= 0x00;
 if (* up_or_down==1)
 {
	int y=15-*position;
	if(y<0)
	{
		 step_down((*position-15),position);
		 *stop=1;
    }
	else 
	{
		step_down(*position,position);
		*stop=1;
	}
 }
else if(* up_or_down==0)
{
	int x=*position-15;
	if (x<0)
	{
		step_up((15-*position),position);
		*stop=1;
	}
	else
	{
		step_up((30-*position),position);
		*stop=1;
	}
}
	
}

int main(void){
DDRC=0x00;/*PORTC is input*/
DDRA=0xFF;//PORTa is Output
DDRB=0x00;
PORTC=0x00;/*PORTC is pull down*/
position=&x;
int k=0;
stop=&l;
GICR = 1<<INT0;		/* Enable INT0*/
MCUCR = 1<<ISC01 | 1<<ISC00;  /* Trigger INT0 on rising edge */
sei();			/* Enable Global Interrupt */
LCD_Init();
    while (1) 
    {
		if (PINC==0x01&&PIND!=0x08)
		{
			if (*position==0)
			{
			*stop = 0;
			}
			else if (*position==15)
			{
				*up_or_down=1;
				step_down(15,position);
				*stop = 0;
				
			}
			else if(*position==30)
			{
			
			*up_or_down=1;
				step_down(30,position);
				*stop = 0;
				
			}
		
		}
			 if (PINC==0x02&&PIND!=0x08)
			{
				if ( *position==0)
				{
					*up_or_down=0;
					step_up(15,position);
					*stop = 0;
				}
				else if (*position==15)
				{
					
					*stop = 0;
				}
				else if(*position==30)
				{
					*up_or_down=1;
					step_down(15,position);
					*stop = 0;
				}
			}
			
			 if (PINC==0x04&&PIND!=0x08)
			{
				if ( *position==0)
				{
					
					*up_or_down=0;
					step_up(30,position);
					*stop = 0;
				}
				else if (*position==15)
				{
					*up_or_down=0;
					step_up(15,position);
					*stop = 0;
					
				}
				else if(*position==30)
				{
					*stop = 0;
					
				}
		    }
			if (PIND==0x08)
			{
				LCD_Clear();
				LCD_String("over load");
				for (int i=0;i>-1;i++)
				{
					if (PIND!=0x08)
					{
						_delay_ms(800);
						break;
					}
				}
			}
		
	}
	}

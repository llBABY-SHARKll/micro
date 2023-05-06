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
int l =0;
 int x=0;
 int* position;
int* up_or_down;//1 =move down 0=move up
int *stop;


void step_up(int x,int* y)
{
	int t=1500;
	for (int i=0;i<x ;i++)
	{
		if (*stop==1)
		{
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
	}
}

void step_down(int x,int* y)
{
	int t=1500;
	for (int i=0;i<x ;i++)
	{
		if (*stop==1)
		{
			
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
DDRA=0xFF;//PORTB is Output
PORTC=0x00;/*PORTC is pull down*/
position=&x;
stop=&l;
GICR = 1<<INT0;		/* Enable INT0*/
MCUCR = 1<<ISC01 | 1<<ISC00;  /* Trigger INT0 on rising edge */

sei();			/* Enable Global Interrupt */



    /* Replace with your application code */
    while (1) 
    {
		if (PINC==0x01)
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
			 if (PINC==0x02)
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
			
			 if (PINC==0x04)
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
		
		
	}
	}

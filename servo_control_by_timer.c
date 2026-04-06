#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

//-90도 39999 * 0.035
#define ANGLE_MINUS90 1400

//0도 39999 * 0.075
#define ANGLE_ZERO 3000

//90도 39999 * 0.115
#define ANGLE_PLUS90 4600


int main(void)
{
	DDRB |= (1<<PB5);
	
	TCCR1A |= (1<<WGM11) | (1<<COM1A1);
	
	TCCR1B |= (1<<WGM12) | (1<<WGM13) | (1<<CS11);
	
	ICR1 = 39999;
	
	while(1)
	{
		OCR1A = ANGLE_MINUS90;
		_delay_ms(1000);
		
		OCR1A = ANGLE_ZERO;
		_delay_ms(1000);
		
		OCR1A = ANGLE_PLUS90;
		_delay_ms(1000);
	}
}




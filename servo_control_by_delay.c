#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define LEFT 1
#define CENTER 2
#define RIGHT 3

void servo(int position)
{
	if(position == LEFT){
		PORTB |= 1 << 7; _delay_us(700);
		PORTB &= ~(1<<7); _delay_ms(18); _delay_us(300);
	}
	
	else if (position == CENTER){
		PORTB |= 1<<7; _delay_ms(1); _delay_us(500);
		PORTB &= ~(1<<7); _delay_ms(17); _delay_us(500);
	}
	
	else{
		PORTB |= 1<<7; _delay_ms(2); _delay_us(300);
		PORTB &= ~(1<<7); _delay_ms(16); _delay_us(700);
	}
}

int main(void)
{
	int i;
	DDRB = 0xff;
	while(1)
	{
		for (i=0; i<50; i++) servo(LEFT);
		for (i=0; i<50; i++) servo(CENTER);
		for (i=0; i<50; i++) servo(RIGHT);
		for (i=0; i<50; i++) servo(CENTER);
	}
}


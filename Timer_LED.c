#include <avr/io.h>
#include <avr/interrupt.h>

volatile int tick_cnt = 0;

void LED_Toggle()
{
	if(++tick_cnt >= 500)
	{
		tick_cnt = 0;
		PORTA ^= 0xFF;
	}
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 0x06;
	LED_Toggle();
}

void init()
{
	DDRA = 0xFF;
	TCNT0 = 0x06;
	TCCR0 = 4;
	TIMSK = 1;
}

int main()
{
	init();
	sei();
	
    while (1) 
    {
    }
}

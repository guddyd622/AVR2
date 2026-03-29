#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define DO 0
#define RE 1
#define MI 2
#define FA 3
#define SOL 4
#define RA 5
#define SI 6
#define DDO 7
#define EOS -1
#define ON 0
#define OFF 1

volatile int state, tone;

char f_table[8]={17, 43, 66, 77, 97, 114, 130, 137};
int song[]={SOL, MI, MI, SOL, MI, DO, RE, MI, RE, DO, MI, SOL,
DDO, SOL, DDO, SOL, DDO, SOL, MI, SOL, RE, FA, MI, RE, DO, EOS};

ISR(TIMER0_OVF_vect)
{
	if(state==ON)
	{
		PORTB=0x00;
		state=OFF;
	}
	else
	{
		PORTB=0x10;
		state=ON;
	}
	TCNT0=f_table[tone];
}

int main()
{
	int i=0;
	DDRB=0x10;
	TCCR0=0x03;
	TIMSK=0x01;
	TCNT0=f_table[song[i]];
	sei();

	do
	{
		tone=song[i++];
		_delay_ms(500);
	}
	while(tone != EOS);	
}

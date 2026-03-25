#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#define F_CPU 16000000
#include <util/delay.h>

volatile int tick_cnt = 0;
volatile int tick_GUGU = 0;

unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67};
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
unsigned char fnd[4];
int n1=1;
int n2=1;
int n3=0;
int n4=0;

int c = 0;

int i=0, count=0, j=0;

void LED_Toggle()
{
	if(++tick_cnt >= 500)
	{
		tick_cnt = 0;
		PORTA ^= 0xFF;
	}
}

void init()
{
	DDRA = 0xFF;
	TCNT0 = 0x06;
	TCCR0 = 4;
	TIMSK = 1;
	DDRC = 0xff;
	DDRG = 0x0f;
}

void FND(int n1, int n2, int n3, int n4)
{
	fnd[3] = n1;
	fnd[2] = n2;
	fnd[1] = n3;
	fnd[0] = n4;
	
	for (j=0; j<10; j++){
		for (i=0; i<4; i++){
			PORTC = digit[fnd[i]];
			PORTG = fnd_sel[i];
			_delay_ms(2);
			if(i%2) _delay_ms(1);
		}
	}
}

void GUGU()
{
	if(++tick_GUGU >= 1000)
	{
		tick_GUGU = 0;
	n2++;
	
	if(n2 == 10) {n1++; n2 = 1;}
	
	if(n1*n2 == 81) {n1 = 1; n2 = 1;}
	
	
	n3=(n1*n2)/10;
	n4=(n1*n2)%10;
	}
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 0x06;
	LED_Toggle();
	GUGU();
}

int main()
{
	init();
	sei();
	
	while (1)
	{	
		FND(n1, n2, n3, n4);
		
	}
	return 0;
}


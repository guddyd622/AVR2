#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile unsigned char c = 0;

static int putchar0(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);

//-90도 39999 * 0.035
#define ANGLE_MINUS90 1400

//0도 39999 * 0.075
#define ANGLE_ZERO 3000

//90도 39999 * 0.115
#define ANGLE_PLUS90 4600



void init_uart(){
	UBRR0H = 0;
	UBRR0L = 8; //16Mhz, 115200baud의 경우
	UCSR0B = 0x98; //RXC interrupt, Receive(Rx) Enable, Transmit(Tx) Enable
	UCSR0C = 0x06; //UART Mode, 8 Bit Data, No Parity,1 Stop Bit
	stdout = &mystdout;
}

ISR(USART0_RX_vect){
	cli();
	c = UDR0;
	sei();
	
	printf("%d\n", c);
}

int putchar0(char c, FILE *stream){
	if(c=='\n')
	putchar0('\r', stream);
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0=c;
	return 0;
}

int main(void)
{
	sei();
	init_uart();
	
	DDRB |= (1<<PB5);
	TCCR1A |= (1<<WGM11) | (1<<COM1A1);
	TCCR1B |= (1<<WGM12) | (1<<WGM13) | (1<<CS11);
	ICR1 = 39999;
	
	while(1)
	{
		if(c == '1')
		OCR1A = ANGLE_PLUS90;
	}
}




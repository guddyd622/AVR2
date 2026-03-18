#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

volatile unsigned char c = 0;

static int putchar0(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);

void init_uart(){
	UBRR0H = 0;
	UBRR0L = 8;
	UCSR0B = 0x98;
	UCSR0C = 0x06;
	stdout = &mystdout;
}

int main()
{
	init_uart();
	sei();
	
	printf("Input Number of gugudan : ");
	while(1);
}

ISR(USART0_RX_vect){
	cli();
	c = UDR0;
	
	printf("%c\n",c);
	c = c - '0';
	
	if(c >= 1 && c <= 9){
		for(int i = 1; i <= 9; i++)
			printf("\t%d x %d = %d\n", c, i, c*i);
	}
	printf("Input Number of gugudan : ");
	sei();
}

int putchar0(char c, FILE *stream){
	if(c=='\n')
		putchar0('\r', stream);
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0=c;
	return 0;
}

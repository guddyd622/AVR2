#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#define F_CPU 16000000
#include <util/delay.h>

volatile unsigned char c = 0;

static int putchar0(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);

unsigned short read_ADC();
void show_adc(unsigned short value);

void init(){
	UBRR0H = 0;
	UBRR0L = 8;
	UCSR0B = 0x98;
	UCSR0C = 0x06;
	stdout = &mystdout;
	ADMUX = 0x00;
	ADCSRA = 0x87;
}

int main()
{
	
	unsigned short value;
	DDRA = 0xFF;
	
	init();
	sei();
	
	while(1)
	{
		value = read_ADC();
		show_adc(value);
		printf("test : %d\n", (value - 700)/2);
		_delay_ms(10);
	}
	
}

/*ISR(USART0_RX_vect){
	cli();
	c = UDR0;
	
	printf("%c\n",c);
	c = c - '0';
	
	printf("test : %d", value);
	sei();
}*/

int putchar0(char c, FILE *stream){
	if(c=='\n')
		putchar0('\r', stream);
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0=c;
	return 0;
}

unsigned short read_ADC()
{
	volatile unsigned char adc_low, adc_high;
	volatile unsigned short value;
	ADCSRA |= (1<<ADSC);

	while((ADCSRA & (1<<ADIF)) == 0);

	adc_low = ADCL;
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;
	return value;
}

void show_adc(unsigned short value)
{
	if(value <= 843)
	PORTA = 0xFF;
	else if (value <= 879)
	PORTA = 0x7F;
	else if (value <= 899)
	PORTA = 0x3F;
	else if (value <= 919)
	PORTA = 0x1F;
	else if (value <= 939)
	PORTA = 0x0F;
	else if (value <= 959)
	PORTA = 0x07;
	else if (value <= 979)
	PORTA = 0x03;
	else if (value <= 999)
	PORTA = 0x01;
	else
	PORTA = 0x00;
}

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#define CDS_1 833
#define CDS_10 901
#define CDS_100 989
void init_adc();
unsigned short read_ADC();
void show_adc(unsigned short value);

int main()
{
	unsigned short value;
	DDRA = 0xFF;
	init_adc();

	while(1)
	{
		value = read_ADC();
		show_adc(value);
		_delay_ms(10);
	}
}


void init_adc()
{
	ADMUX = 0x00;
	ADCSRA = 0x87;
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

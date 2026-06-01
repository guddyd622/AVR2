#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "GLCD_Eng.h"
#include "GLCD_Draw.h"
#define V_REF 5.0f

//float volt;
unsigned int TimeDiv = 0;

unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67};
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
unsigned char fnd[4];

void adc_init()
{
	ADMUX = 0x47;	//ADC7 -> PF7
	ADCSRA = 0x87;
}

void fnd_init()
{
	DDRC = 0xff;
	DDRG = 0x0f;
}

unsigned short read_volt()
{
	static float volt_f = 0.0f;
	static unsigned char first_run = 1;
	
	volatile unsigned char adc_low, adc_high;
	volatile unsigned short value;
	ADCSRA |= (1<<ADSC);

	while((ADCSRA & (1<<ADIF)) == 0);

	adc_low = ADCL;
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;
	
	if (first_run) {
		volt_f = (float)value;
		first_run = 0;
	} 
	else 
	volt_f = 0.9f * volt_f + 0.1f * value;
	
	return (unsigned short)volt_f / 1024.0f * V_REF;
}

void showFND(int volt)
{
	int milvolt = volt *1000;
	fnd[3] = (milvolt/1000)%10;
	fnd[2] = (milvolt/100)%10;
	fnd[1] = (milvolt/10)%10;
	fnd[0] = milvolt%10;
		for (int i=0; i<4; i++){
			
			if(i == 3)
			PORTC = digit[fnd[i]] | 0x80;
			else
			PORTC = digit[fnd[i]];
			
			PORTG = fnd_sel[i];
			_delay_ms(2);
			if(i%2) _delay_ms(1);
	}
}

void axis()
{
	GLCD_Line(60, 17, 60, 120);
	GLCD_Line(0, 20, 63, 20);
	GLCD_String(0, 0, "HV");
	GLCD_String(7, 0, "0V");
}

int main(void)
{
	_delay_ms(50); // wait for system stabilization
	
	adc_init();
	fnd_init();
	GLCD_Port_Init(); // initialize MCU and kit
	GLCD_Init();
	
	
	axis();
	
	while(1)
	{
		int volt = read_volt();
		
		showFND(volt);
	}
}

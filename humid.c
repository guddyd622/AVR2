#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67};
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
unsigned char fnd[4];

float value_f = 0;

#define DRY_SOIL	300   // 마른 토양
#define HUMID_SOIL	700   // 습한 토양
#define IN_WATER	950  // 물 속에 있음

void init_adc();
unsigned short read_adc();
void show_adc(unsigned short value);

int main()
{
	unsigned short value;
	DDRA = 0xff;
	DDRC = 0xff;
	DDRG = 0x0f;
	init_adc();
	while(1)
	{
		value = read_adc();
		show_adc(value);
	}
}
void init_adc()
{
	ADMUX = 0x00;
	// REFS(1:0) = “00” AREF(+5V) 기준전압 사용
	// ADLAR = ‘0’ 디폴트 오른쪽 정렬
	// MUX(4:0) = “00000” ADC0 사용, 단극 입력
	ADCSRA = 0x87;
	// ADEN = ‘1’  ADC를 Enable
	// ADFR = ‘0’ single conversion 모드
	// ADPS(2:0) = “111” 프리스케일러 128분주
}
unsigned short read_adc()
{
	unsigned char adc_low, adc_high;
	float value;
	ADCSRA |= (1<<ADSC); 	// ADC start conversion, ADSC = ‘1’
	while ((ADCSRA & (1<<ADIF)) == 0) // ADC 변환 완료(ADIF) 검사
	;
	adc_low = ADCL;		// 변환된 Low 값 읽어오기 (반드시 먼저!)
	adc_high = ADCH;	// 변환된 High 값 읽어오기
	value = (adc_high << 8) | adc_low;
	// value 는 High 및 Low 연결 16비트값
	value_f = 0.99f*value_f + 0.01f*value;
	return (int)value_f;
}

void show_adc(unsigned short value)
{
	fnd[3] = (value/1000)%10;
	fnd[2] = (value/100)%10;
	fnd[1] = (value/10)%10;
	fnd[0] = value%10;
		for(int i=0;i<4;i++)
		{
			PORTC = digit[fnd[i]];
			PORTG = fnd_sel[i];
			_delay_ms(2);
			if(i%2) _delay_ms(1);
		}

}

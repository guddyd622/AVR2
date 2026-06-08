#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

// X, Y 좌표는 ADC 위해 아날로그 입력핀으로 연결해야함
// PORTF 1번 포트(좌-우 좌표) : S-X (왼쪽: 1023 오른쪽: 0)
// PORTF 2번 포트(상-하 좌표): S-Y (위: 1023 아래: 0)
// 조이스틱 눌렀을 때는 0과 1로 입력되므로 디지털 핀 사용
// 조이스틱 누르면 0, 안 누르면 1
// PORTG 4번 포트(스위치): S-K

static int putchar0(unsigned char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);
volatile unsigned char c=0;

volatile unsigned char adc_low, adc_high;
volatile unsigned short value;
volatile unsigned short x, y;

// printf 쓰기 위해 필요
static int putchar0(unsigned char c, FILE *stream) {
	if(c=='\n')
	putchar0('\r', stream);
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0=c;
	return 0;
}

#define X 1
#define Y 2

// ADC 처음 결과는 X좌표, 다음 결과는 Y좌표로 번갈아서 나옴
// 그래서 x_y = X 와 x_y = Y를 반복함
// 처음에는 (x_y = X) ADC1번에서 X좌표 가져옴
// 다음은 (x_y = Y) ADC2번에서 Y좌표 가져옴

volatile unsigned char x_y = X;

ISR(ADC_vect){
	cli();
	adc_low = ADCL;
	adc_high = ADCH;
	value = (adc_high << 8) | adc_low;
	if(x_y == X){
		x = value;  // X 값 저장
		x_y = Y; // 다음 ADC 값은 Y임
		ADMUX = 0x02; // ADC2 (joystick y)
		} else {
		y = value;
		x_y = X;
		ADMUX = 0x01; // ADC1 (joystick x)
	}
	printf("x = %4d y = %4d (pushed = %d)\n",x, y, PING >> 4);
	_delay_ms(100);
	sei();
}

void init_adc() {
	ADMUX = 0x01; // ADC1 (joystick x)
	ADCSRA = 0x8F;  //10001111 ADEN(7번 비트), ADIE(3번 비트 Interrupt enable), ADPS(2~0번 비트: 128분주)
}

void init_uart(){
	// ****** 시리얼통신 설정
	UBRR0H=0;
	UBRR0L=8; // 16MHz, 115200 baud
	UCSR0B=0x98; // Rx Complete Interrupt Enable, RX/TX Enable
	UCSR0C=0x06; // UART Mode, 8 bit data, No parity, 1 Stop bit
	stdout = &mystdout;
	sei();
}

int main() {
	init_adc();
	init_uart();
	DDRG = 0x00; // PORTG4 입력으로 정의(조이스틱 누름 여부)
	while(1) {
		ADCSRA |= (1<<ADSC);  // ADSC (ADC Start Conversion: 6번 비트)
	}
}

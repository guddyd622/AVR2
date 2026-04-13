#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#define OCR1_R OCR1A
#define OCR1_G OCR1B
#define OCR1_B OCR1C

unsigned char redV;     // 빨간 LED 아날로그 값 설정 (0~255)
unsigned char greenV;   // 초록 LED 아날로그 값 설정 (0~255)
unsigned char blueV;    // 파란 LED 아날로그 값 설정 (0~255)

volatile unsigned char c = 0;

static int putchar0(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);

int x = 0;
int y = 0;

void set_color(unsigned char red, unsigned char green, unsigned char blue) {
	OCR1_R = red;
	OCR1_G = green;
	OCR1_B = blue;
}

void timer_init() {
	// Fast PWM 8bit
	// WGM13(0), WGM12(1), WGM11(0), WGM10(1)
	// COM1A1/COM1B1/COM1C1 : 1, COM1A0/COM1B0/COM1C0 : 0
	TCCR1A |= _BV(WGM10) | _BV(COM1A1) | _BV(COM1B1) | _BV(COM1C1);
	TCCR1B |= _BV(WGM12) | _BV(CS11);
	set_color(0, 0, 0);
}

ISR(USART0_RX_vect){
	c = UDR0;
	TIMSK = 0x01;
	
	if(c == 'w' || 'W')
	y++;
	else if(c == 's' || 'S')
	y--;
	else if(c == 'a' || 'A')
	x--;
	else if(c == 'd' || 'D')
	x++;
	
	
}


void init_uart(){
	UBRR0H = 0;
	UBRR0L = 8; //16Mhz, 115200baud의 경우
	UCSR0B = 0x98; //RXC interrupt, Receive(Rx) Enable, Transmit(Tx) Enable
	UCSR0C = 0x06; //UART Mode, 8 Bit Data, No Parity,1 Stop Bit
}

int main(void) {
	DDRB = _BV(5) | _BV(6) | _BV(7); // PB5, PB6, PB7을 출력으로 설정
	timer_init();
	init_uart();
	sei();

	while (1) {
		// 1. 빨간색 사이클
		greenV = 0; blueV = 0;
		for (redV = 0; redV <= 250; redV += 5) {
			set_color(redV, greenV, blueV);
			_delay_ms(30);
		}
		for (redV = 255; redV >= 5; redV -= 5) {
			set_color(redV, greenV, blueV);
			_delay_ms(30);
		}

		// 2. 초록색 사이클
		redV = 0; blueV = 0;
		for (greenV = 0; greenV <= 250; greenV += 5) {
			set_color(redV, greenV, blueV);
			_delay_ms(30);
		}
		for (greenV = 255; greenV >= 5; greenV -= 5) {
			set_color(redV, greenV, blueV);
			_delay_ms(30);
		}

		// 3. 파란색 사이클
		redV = 0; greenV = 0;
		for (blueV = 0; blueV <= 250; blueV += 5) {
			set_color(redV, greenV, blueV);
			_delay_ms(30);
		}
		for (blueV = 255; blueV >= 5; blueV -= 5) {
			set_color(redV, greenV, blueV);
			_delay_ms(30);
		}
	}
}

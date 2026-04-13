#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define OCR1_R OCR1A
#define OCR1_G OCR1B
#define OCR1_B OCR1C

unsigned char redV;     // 빨간 LED 아날로그 값 설정 (0~255)
unsigned char greenV;   // 초록 LED 아날로그 값 설정 (0~255)
unsigned char blueV;    // 파란 LED 아날로그 값 설정 (0~255)

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

int main(void) {
	DDRB = _BV(5) | _BV(6) | _BV(7); // PB5, PB6, PB7을 출력으로 설정
	timer_init();

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

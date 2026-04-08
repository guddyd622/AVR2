#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define TRIG			2 //Trig sig out
#define ECHO			3 // Echo sig out

#define FND_DB_DDR		DDRC
#define FND_DB_PORT		PORTC

#define FND_CT_DDR		DDRG
#define FND_CT_PORT		PORTG

#define SONIC_DDR		DDRB
#define SONIC_PORT		PORTB
#define SONIC_PIN		PINB

#define TRIG_HIGH		(1<<TRIG)
#define TRIG_LOW		~(1<<TRIG)
#define ECHO_HIGH		(1<<ECHO)
#define ECHO_LOW		~(1<<ECHO)

unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67};
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
unsigned char fnd[4];

// FND에 숫자를 표시하는 함수
void display_fnd(unsigned int value)
{
	int i;

	fnd[3] = (value / 1000) % 10;
	fnd[2] = (value / 100) % 10;
	fnd[1] = (value / 10) % 10;
	fnd[0] = (value / 1) % 10;

	for(i = 0; i < 4; i++){
		FND_DB_PORT = digit[fnd[i]];
		FND_CT_PORT = fnd_sel[i];
		_delay_ms(2);
		if(i % 2) _delay_ms(1);
	}
}

int main(void)
{
	unsigned int distance;
	FND_DB_DDR = 0xFF;
	FND_CT_DDR = 0x0F;

	// 포트 B의 TRIG 포트(2번핀)은 출력, ECHO 포트(3번핀)는 입력으로 설정
	SONIC_DDR |= (TRIG_HIGH & ECHO_LOW);
	// OR 연산을 하는 이유는 포트 B 2, 3번 핀 외의 다른 핀은 건드리지 않기 위해서임.

	while(1){
		// 1. Trigger 펄스 공급
		SONIC_PORT &= ~TRIG_HIGH; // Trig = LOW
		_delay_us(10);
		SONIC_PORT |= TRIG_HIGH;  // Trig = HIGH
		_delay_us(10);
		SONIC_PORT &= ~TRIG_HIGH; // Trig = LOW
	// 2. ECHO 입력으로 거리 계산
	TCCR1B = 0x02; // Timer/Counter 1 클럭 0.5us

	while(!(SONIC_PIN & ECHO_HIGH)); // wait until Echo = HIGH;
	TCNT1 = 0x0000; // Timer/Counter 1 값 동작 시작

	while(SONIC_PIN & ECHO_HIGH); // wait until Echo = LOW;
	TCCR1B = 0x00; // Timer/Counter 1 값 동작 정지

	// TCNT1에는 Echo 신호가 HIGH로 유지된 카운트 수 저장
	distance = (unsigned int)(TCNT1 / 2 / 58);
	// 시간에 비례하는 거리 보정 함수 적용
	display_fnd(distance);
}
}

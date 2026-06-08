#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>

// IN1 PORTB0
// IN2 PORTB1
// IN3 PORTB2
// IN4 PORTB3

unsigned char wave_drive[4] = {0x01, 0x02, 0x04, 0x08};
unsigned char full_step_drive[4] = {0x0C, 0x06, 0x03, 0x09};
unsigned char half_step_drive[8] = {0x08,0x0C,0x04,0x06,0x02,0x03,0x01,0x09};

int step_index = -1;

// 0001-0010-0100-1000
unsigned char wave_forward(){
	step_index++;
	if(step_index >= 4)
	step_index = 0;
	return wave_drive[step_index];
}

// 1000-0100-0010-0000
unsigned char wave_backward(){
	step_index--;
	if(step_index < 0)
	step_index = 3;
	return wave_drive[step_index];
}

// 0x08->0x0C->0x04->0x06->0x02->0x03->0x01->0x09
unsigned char full_forward(){
	step_index++;
	if(step_index >= 4)
	step_index = 0;
	return full_step_drive[step_index];
}

// full_forward()과 패턴 거꾸로 0x09부터 0x08까지
unsigned char full_backward(){
	step_index--;
	if(step_index < 0)
	step_index = 3;
	return full_step_drive[step_index];
}

// 0x0C->0x06->0x03->0x09
unsigned char half_forward(){
	step_index++;
	if(step_index >= 8)
	step_index = 0;
	return half_step_drive[step_index];
}

// half_forward()과 패턴 거꾸로 0x09부터 0x0C까지
unsigned char half_backward(){
	step_index--;
	if(step_index < 0)
	step_index = 7;
	return half_step_drive[step_index];
}

int main(){
	DDRB = 0x0F;  // PORTB 하위 4비트: 스텝모터 출력
	while(1){
		// 2-Phase Half Step
		//4096:한바퀴(360도), 2048:반바퀴(180도)
		for(int i = 0; i< 4096; i++){
			PORTB = half_forward();
			_delay_ms(5);
		}
		_delay_ms(1000);
		for(int i = 0; i< 4096; i++){
			PORTB = half_backward();
			_delay_ms(5);
		}
		_delay_ms(1000);
	}
}

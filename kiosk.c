#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "GLCD_Eng.h"

#define KEY_DDR DDRF
#define KEY_PORT PORTF
#define KEY_PIN PINF

unsigned char key_scan(unsigned char a);
int key_convert(unsigned char in);

int main(void)
{
	// 키패드 관련 변수
	unsigned char in, in0, in1, in2, in3;
	int key_val = 0;
	int prev_key = 0;
	
	// 키오스크 관련 변수
	unsigned int total_price = 0;

	_delay_ms(50);      // 시스템 안정화 대기
	GLCD_Port_Init();   // MCU 포트 초기화
	GLCD_Init();        // GLCD 초기화
	GLCD_Clear();       // GLCD 화면 지우기

	// 초기 키오스크 메뉴 화면 출력
	GLCD_String(0, 0, "1.Americano 150");
	GLCD_String(1, 0, "2.Latte 300");
	GLCD_String(2, 0, "3.Juice 400");
	GLCD_String(3, 0, "4.Reset");
	GLCD_String(5, 0, "Total:    0");

	while (1)
	{
		// 키패드 스캔
		in0 = key_scan(0x01);
		in1 = key_scan(0x02);
		in2 = key_scan(0x04);
		in3 = key_scan(0x08);

		in = (in3 | in2 | in1 | in0); // 4줄 입력 값 취합
		key_val = key_convert(in);    // 키 입력 값을 숫자로 변환

		// 유효한 키가 눌렸을 경우
		if(key_val >= 1 && key_val <= 16) {
			// 버튼을 계속 누르고 있을 때 중복 연산 방지 (디바운싱 엣지 검출)
			if(key_val != prev_key) {
				if(key_val == 1) {
					total_price += 150;
				}
				else if(key_val == 2) {
					total_price += 300;
				}
				else if(key_val == 3) {
					total_price += 400;
				}
				else if(key_val == 4) {
					total_price = 0; // 초기화
				}

				// 총액 디스플레이 업데이트
				GLCD_xy(5, 7); // Total: 문자열 뒷부분으로 커서 이동
				GLCD_4DigitDecimal(total_price); // 4자리 10진수로 총액 출력
				
				prev_key = key_val; // 현재 눌린 키 저장
				_delay_ms(50);      // 바운싱 방지용 짧은 딜레이
			}
		}
		else {
			// 키를 떼었을 때 상태 초기화
			prev_key = 0;
		}
	}
}

// 키패드 스캔 함수
unsigned char key_scan(unsigned char a)
{
	unsigned char in;

	KEY_DDR = 0xFF;
	KEY_PORT = a;
	KEY_DDR = 0x0F;
	_delay_us(50);

	in = (PINF & 0xF0);
	if(in != 0x00)
	{
		in |= a;
		return in;
	}
	return 0;
}

// 키 변환 함수
int key_convert(unsigned char in)
{
	int returnvalue;
	switch(in)  {
		case 0x24: returnvalue = 1; break;
		case 0x14: returnvalue = 2; break;
		case 0x84: returnvalue = 3; break;
		case 0x44: returnvalue = 4; break;
		case 0x28: returnvalue = 5; break;
		case 0x18: returnvalue = 6; break;
		case 0x88: returnvalue = 7; break;
		case 0x48: returnvalue = 8; break;
		case 0x21: returnvalue = 9; break;
		case 0x11: returnvalue = 10; break;
		case 0x81: returnvalue = 11; break;
		case 0x41: returnvalue = 12; break;
		case 0x22: returnvalue = 13; break;
		case 0x12: returnvalue = 14; break;
		case 0x82: returnvalue = 15; break;
		case 0x42: returnvalue = 16; break;
		default: returnvalue = 0; break;
	}
	return returnvalue;
}

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

// 비트번호에 이름 정의
#define DIN 2
#define CS 1
#define CLK 0
#define DOT_PORT PORTG
#define DOT_DDR DDRG

// x: 레지스터, y: 비트번호(비트이름)
// 예: SetBit(PORTC, DIN)
#define SetBit(x,y) (x|=(1<<y))
#define ClrBit(x,y) (x&=~(1<<y))

unsigned char Mini_num[10][8] = {
	{0x70,0x50,0x50,0x50,0x70,0x00,0x00,0x00}, //0
	{0x20,0x60,0x20,0x20,0x70,0x00,0x00,0x00}, //1
	{0x70,0x10,0x70,0x40,0x70,0x00,0x00,0x00}, //2
	{0x70,0x10,0x70,0x10,0x70,0x00,0x00,0x00}, //3
	{0x50,0x50,0x70,0x10,0x10,0x00,0x00,0x00}, //4
	{0x70,0x40,0x70,0x10,0x70,0x00,0x00,0x00}, //5
	{0x70,0x40,0x70,0x50,0x70,0x00,0x00,0x00}, //6
	{0x70,0x50,0x50,0x10,0x10,0x00,0x00,0x00}, //7
	{0x70,0x50,0x70,0x50,0x70,0x00,0x00,0x00}, //8
	{0x70,0x50,0x70,0x10,0x70,0x00,0x00,0x00}  //9
};

void SPI_Write_Byte(unsigned char num) {
	unsigned char t, Mask, Flag;
	Mask = 128;
	for(t=0; t<8; t++) {
		ClrBit(DOT_PORT, CLK);
		Flag = num & Mask;
		if(Flag==0) ClrBit(DOT_PORT, DIN);
		else SetBit(DOT_PORT, DIN);
		SetBit(DOT_PORT, CLK);
		Mask = Mask >> 1;
	}
}

void MAX7219_INIT() {
	DOT_DDR = 0xFF;
	// Set BCD decode mode
	ClrBit(DOT_PORT, CS); // CS pin is pulled LOW
	SPI_Write_Byte(0x09); // Select Decode Mode register
	SPI_Write_Byte(0x00); // Select BCD mode for digits DIG0-DIG7
	SetBit(DOT_PORT, CS); // CS pin is pulled HIGH

	// Set display brightness
	ClrBit(DOT_PORT, CS); // CS pin is pulled LOW
	SPI_Write_Byte(0x0A); // Select Intensity register
	SPI_Write_Byte(0x00); // Set brightness (0x00~0x0F)
	SetBit(DOT_PORT, CS); // CS pin is pulled HIGH

	// Set display refresh
	ClrBit(DOT_PORT, CS); // CS pin is pulled LOW
	SPI_Write_Byte(0x0B); // Select Scan-Limit register
	SPI_Write_Byte(0x07); // Select digits DIG0-DIG3
	SetBit(DOT_PORT, CS); // CS pin is pulled HIGH

	// Turn on the display
	ClrBit(DOT_PORT, CS); // CS pin is pulled LOW
	SPI_Write_Byte(0x0C);
	SPI_Write_Byte(0x01);
	SetBit(DOT_PORT, CS); // CS pin is pulled HIGH

	// Disable Display-Test
	ClrBit(DOT_PORT, CS); // CS pin is pulled LOW
	SPI_Write_Byte(0x0F); // Select Display-Test register
	SPI_Write_Byte(0x00); // Disable Display-Test
	SetBit(DOT_PORT, CS); // CS pin is pulled HIGH
}

void Write_Byte(unsigned char myColumn, unsigned char myValue) {
	ClrBit(DOT_PORT, CS); // select max7219.
	SPI_Write_Byte(myColumn); // send myColumn value to max7219 (digit place).
	SPI_Write_Byte(myValue); // send myValue value to max7219 (digit place).
	SetBit(DOT_PORT, CS); // deselect max7219.
}

// This is clear matrix function.
void Clear_Matrix(void) {
	unsigned char x;
	for(x=1; x<9; x++) {
		Write_Byte(x, 0x00);
	}
}

// 매개변수: 두 자리까지의 정수
void Write_Two_Mini_Num(unsigned char myChar) {
	unsigned char Column, Ten_num, One_num;
	Clear_Matrix();
	Ten_num = (myChar/10)%10;
	One_num = myChar%10;
	for(Column=0; Column<8; Column++) {
		Write_Byte(Column+1, Mini_num[Ten_num][Column] | (Mini_num[One_num][Column] >> 4));
	}
}

#define KEY_DDR DDRF
#define KEY_PORT PORTF
#define KEY_PIN PINF

unsigned char key_scan(unsigned char a);
int key_convert(unsigned char in);

int main(void) {
	// 도트매트릭스 관련
	MAX7219_INIT(); // initialize max7219
	Clear_Matrix();

	//키패드관련
	unsigned char in, in0, in1, in2, in3;

	while(1) {
		// 키패드 첫번째줄 스캔
		in0 = key_scan(0x01);
		// 키패드 두번째줄 스캔
		in1 = key_scan(0x02);
		// 키패드 세번째줄 스캔
		in2 = key_scan(0x04);
		// 키패드 네번째줄 스캔
		in3 = key_scan(0x08);

		in = (in3 | in2 | in1 | in0); //4줄 입력 값 취합
		in = key_convert(in); //키 입력 값을 숫자로 변환

		if(in >= 1 && in <= 16) {
			Write_Two_Mini_Num(in);
		}
	}
}

//키패드에 각 줄에 입력 값이 있는지 스캔하는 함수
unsigned char key_scan(unsigned char a) {
	unsigned char in;

	KEY_DDR = 0xFF; //all out
	KEY_PORT = a; //키패드 줄 위치
	KEY_DDR = 0x0F; //Port 7bit~4bit(In) 3bit~0bit(Out)

	_delay_us(50);

	// 키패드 입력 포트의 상위 4자리 값 추출
	in = (PINF & 0xF0);

	if(in != 0x00) {
		in |= a;
		return in;
	}
	return 0;
}

int key_convert(unsigned char in) {
	int returnvalue;

	switch(in) {
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

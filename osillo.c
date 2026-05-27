#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "GLCD_Eng.h"
#include "GLCD_Draw.h"
int main(void)
{
	unsigned char i;
	_delay_ms(50); // wait for system stabilization
	GLCD_Port_Init(); // initialize MCU and kit
	GLCD_Init();
	while(1)
	{
		GLCD_Line(60, 17, 60, 120);
		GLCD_Line(0, 20, 63, 20);
		GLCD_String(0, 0, "5V");
		GLCD_String(7, 0, "0V");
	}
}

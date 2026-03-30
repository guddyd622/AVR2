#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define DO 0
#define RE 1
#define MI 2
#define FA 3
#define SOL 4
#define RA 5
#define SI 6
#define DDO 7
#define SLEEP 8 // f_table에서 묵음을 위한 카운트 저장 인덱스
#define EOS -1
#define ON 0
#define OFF 1
volatile int state=OFF, tone=0;
char f_table[10] = {17, 43, 66, 77, 97, 114, 130, 137, 255, 0};
ISR(TIMER0_OVF_vect)
{
	if (state == ON) {
		PORTB = 0x00;
		state = OFF;
	}
	else {
		PORTB = 0x10;
		state = ON;
	}
	TCNT0 = f_table[tone];
	
	int i = 0;
	
	if(i++ == 1000){
	tone = 8;
	i = 0;
	}
}

ISR(USART0_RX_vect){
	char c = UDR0;
	TIMSK = 0x01;
	tone = c-48;
	//TCNT0 = f_table[c];
}

void init_uart(){
	UBRR0H = 0;
	UBRR0L = 8; //16Mhz, 115200baud의 경우
	UCSR0B = 0x98; //RXC interrupt, Receive(Rx) Enable, Transmit(Tx) Enable
	UCSR0C = 0x06; //UART Mode, 8 Bit Data, No Parity,1 Stop Bit
}

int main()
{
	DDRB = 0x10;
	TCCR0 = 0x03; // Normal Mode, 32분주
	//TCNT0 = f_table[song[i]];
	init_uart();
	sei();
	while(1){}
}


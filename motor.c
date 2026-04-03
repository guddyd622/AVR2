#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define LOW 128
#define MID 77
#define HIGH 26
#define OFF 255

volatile unsigned char c = 0;
volatile unsigned int count = 0;

static int putchar0(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);

ISR(TIMER2_COMP_vect)
{
    count++;
    if(count >= 1000) {
        count = 0;
    }
}

int putchar0(char c, FILE *stream)
{
    if(c == '\n') putchar0('\r', stream);
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
    return 0;
}

void init_uart()
{
    UBRR0H = 0;
    UBRR0L = 8;
    UCSR0B = 0x98;
    UCSR0C = 0x06;
    stdout = &mystdout;
}

ISR(USART0_RX_vect)
{
    c = UDR0;
    printf("%c\n", c);

    if(c == '1') {
        OCR2 = OFF;
        printf("Motor Off\n");
    }
    else if(c == '2') {
        OCR2 = LOW;
        printf("Motor Speed: LOW\n");
    }
    else if(c == '3') {
        OCR2 = MID;
        printf("Motor Speed: MID\n");
    }
    else if(c == '4') {
        OCR2 = HIGH;
        printf("Motor Speed: HIGH\n");
    }
}

int main()
{
    DDRB = 0xFF;
    TCCR2 = 0x7B;
    TIMSK = 0x80;
    OCR2 = OFF;
    init_uart();
    sei();

    printf(" 1. Motor Off\n 2. Motor Speed: LOW\n 3. Motor Speed: MID\n 4. Motor Speed: HIGH\n");

    while(1)
    {
        // 무한 루프
    }
}

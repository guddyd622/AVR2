#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char c = 0;
static int putchar0(unsigned char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);

volatile unsigned char x = 0, y = 0, target_x = 0, target_y = 0;

#define OCR1_R OCR1A
#define OCR1_G OCR1B
#define OCR1_B OCR1C

void set_color(unsigned char red, unsigned char green, unsigned char blue)
{
    OCR1_R = red;
    OCR1_G = green;
    OCR1_B = blue;
}

void timer_init()
{
    TCCR1A |= _BV(WGM10) | _BV(COM1A1) | _BV(COM1B1) | _BV(COM1C1);
    TCCR1B |= _BV(WGM12) | _BV(CS11);
    set_color(0, 0, 0);
}

static int putchar0(unsigned char c, FILE *stream)
{
    if (c == '\n') putchar0('\r', stream);
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
    return 0;
}

void init_uart() {
    UBRR0H = 0;
    UBRR0L = 8; // 16MHz, 115200 baud
    UCSR0B = 0x98; // Rx Complete Interrupt Enable, RX/TX Enable
    UCSR0C = 0x06; // UART Mode, 8 bit data, No parity, 1 Stop bit
    stdout = &mystdout;
}

ISR(USART0_RX_vect) {
    c = UDR0;
    if (c == 'X' || c == 'x') { printf("Ended\n"); UCSR0B = 0x00; }
    if ((c == 'A' || c == 'a') && x > 0) x--;
    if ((c == 'D' || c == 'd') && x < 9) x++;
    if ((c == 'W' || c == 'w') && y < 9) y++;
    if ((c == 'S' || c == 's') && y > 0) y--;

    printf("x=%d y=%d\n", x, y);
    if (x < 5) {
        if (y < 5) set_color(0, 0, 255);
        else       set_color(255, 0, 0);
    }
    else if (x < 10) {
        if (y < 5) set_color(255, 255, 255);
        else       set_color(0, 255, 0);
    }

    if (target_x == x && target_y == y)
    {
        printf("Success\n");
        target_x = rand() % 10;
        target_y = rand() % 10;
        printf("target_x=%d target_y=%d\n", target_x, target_y);
    }
}

int main()
{
    DDRB = _BV(5) | _BV(6) | _BV(7);
    timer_init();
    init_uart();
    sei();

    target_x = rand() % 10;
    target_y = rand() % 10;
    printf("target_x=%d target_y=%d\n", target_x, target_y);

    while (1) { }
}

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "GLCD_Eng.h"
#include "GLCD_Draw.h"
#define V_REF 5.0f

// 오실로스코프 Time/Div 스텝 설정 (1-2-5 스케일 방식)
#define MAX_TIME_DIV 6
int time_div_steps[MAX_TIME_DIV] = {1, 2, 5, 10, 20, 50};
char* time_div_strs[MAX_TIME_DIV] = {"x1 ", "x2 ", "x5 ", "x10", "x20", "x50"};

volatile int time_div_idx = 2; // 초기 상태 (x5 배율, graph_speed = 5)
volatile int graph_speed = 5;
volatile int update_ui = 1;    // 화면 글씨 갱신 플래그

int graph_col = 21; // 시간축(가로) 진행 좌표

void adc_init()
{
    ADMUX = 0x41;    //ADC7 -> PF1
    ADCSRA = 0x87;
}

/* FND 초기화 주석 처리 유지
void fnd_init()
{
    DDRC = 0xff;
    DDRG = 0x0f;
}
*/

void ext_int_init()
{
    DDRE &= ~(0x30);
    EIMSK = 0x30;
    EICRB = 0x0A;
}

// INT4: Time/Div 축소 (더 빠르게 그래프를 그림 - Zoom In)
ISR(INT4_vect)
{
    cli();
    if (time_div_idx > 0) {
        time_div_idx--;
        graph_speed = time_div_steps[time_div_idx];
        update_ui = 1; // 배율이 바뀌었으므로 화면 갱신 요청
    }
    _delay_ms(50); // 디바운싱
    sei();
}

// INT5: Time/Div 확대 (더 느리게 그래프를 그림 - Zoom Out)
ISR(INT5_vect)
{
    cli();
    if (time_div_idx < (MAX_TIME_DIV - 1)) {
        time_div_idx++;
        graph_speed = time_div_steps[time_div_idx];
        update_ui = 1;
    }
    _delay_ms(50);
    sei();
}

float read_volt()
{
    static float volt_f = 0.0f;
    static unsigned char first_run = 1;
    
    volatile unsigned char adc_low, adc_high;
    volatile unsigned short value;
    ADCSRA |= (1<<ADSC);

    while((ADCSRA & (1<<ADIF)) == 0);

    adc_low = ADCL;
    adc_high = ADCH;
    value = (adc_high << 8) | adc_low;
    
    if (first_run) {
        volt_f = (float)value;
        first_run = 0;
    } else {
        volt_f = 0.0f * volt_f + 1.0f * value;
    }
    
    return (volt_f / 1024.0f) * V_REF;
}

void axis()
{
    GLCD_Line(60, 17, 60, 120);
    GLCD_Line(0, 20, 63, 20);
    GLCD_String(0, 0, "5V");
    GLCD_String(7, 0, "0V");
}

int main(void)
{
    _delay_ms(50);
    
    adc_init();
    ext_int_init();
    
    GLCD_Port_Init();
    GLCD_Init();
    
    axis();
    
    sei();
    
    int loop_count = 0;
    
    int old_row[128];
    for(int i=0; i<128; i++) {
        old_row[i] = -1;
    }
    
    while(1)
    {
        // 배율이 변경되었을 때만 우측 상단에 현재 Time/Div 배율을 텍스트로 표시
       

        float volt = read_volt();
        
        loop_count++;
        if (loop_count >= graph_speed) {
            loop_count = 0;
            
            int graph_row = 59 - (int)((volt / V_REF) * 59.0f);
            if (graph_row < 0) graph_row = 0;
            if (graph_row > 59) graph_row = 59;
            
            if (old_row[graph_col] != -1) {
                GLCD_Dot_Clear(old_row[graph_col], graph_col);
            }
            
            GLCD_Dot(graph_row, graph_col);
            old_row[graph_col] = graph_row;
            
            graph_col++;
            
            if (graph_col > 120) {
                graph_col = 21;
            }
        }
    }
}

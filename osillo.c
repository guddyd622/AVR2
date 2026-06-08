#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "GLCD_Eng.h"
#include "GLCD_Draw.h"
#define V_REF 5.0f

// 오실로스코프 Time/Div 스텝 설정 (1-2-5 스케일 방식)

volatile int time_div_idx = 1; // 초기 상태 (x5 배율, graph_speed = 5)
volatile int graph_speed = 1;
volatile int update_ui = 1;    // 화면 글씨 갱신 플래그

int graph_col = 21; // 시간축(가로) 진행 좌표

void adc_init()
{
    ADMUX = 0x41;    //ADC7 -> PF1
    ADCSRA = 0x87;
}

/*
void fnd_init()
{
    DDRC = 0xff;
    DDRG = 0x0f;
}
*/

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
    
    GLCD_Port_Init();
    GLCD_Init();
    
    axis();
    
    int loop_count = 0;
    
    int old_row[128];
    for(int i=0; i<128; i++) {
        old_row[i] = -1;
    }
    
    while(1)
    {
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

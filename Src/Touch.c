
#include "Touch.h"

#include "LCD.h"
#include "XPT2046.h"
#include <stdlib.h>
#include <math.h>

static tp_dev_t s_tTouch;
uint16_t color=BLACK;


void tp_init(void)
{
	__SPI_128div;
	xpt2046_init();
	__SPI_2div;
}

void tp_draw_touch_point(uint16_t hwXpos, uint16_t hwYpos, uint16_t hwColor)
{
	lcd_draw_line(hwXpos - 12, hwYpos, hwXpos + 13, hwYpos, hwColor);
	lcd_draw_line(hwXpos, hwYpos - 12, hwXpos, hwYpos + 13, hwColor);
	lcd_draw_point(hwXpos + 1, hwYpos + 1, hwColor);
	lcd_draw_point(hwXpos - 1, hwYpos + 1, hwColor);
	lcd_draw_point(hwXpos + 1, hwYpos - 1, hwColor);
	lcd_draw_point(hwXpos - 1, hwYpos - 1, hwColor);
	lcd_draw_circle(hwXpos, hwYpos, 6, hwColor);
}

void tp_draw_big_point(uint16_t hwXpos, uint16_t hwYpos, uint16_t hwColor)
{
	lcd_draw_point(hwXpos, hwYpos, hwColor);
	lcd_draw_point(hwXpos + 1, hwYpos, hwColor);
	lcd_draw_point(hwXpos, hwYpos + 1, hwColor);
	lcd_draw_point(hwXpos + 1, hwYpos + 1, hwColor);
}

void tp_show_info(uint16_t hwXpos0, uint16_t hwYpos0,
                     uint16_t hwXpos1, uint16_t hwYpos1,
                     uint16_t hwXpos2, uint16_t hwYpos2,
                     uint16_t hwXpos3, uint16_t hwYpos3, uint16_t hwFac)
{

	lcd_display_string(40, 160, (const uint8_t *)"x1", 16, RED);
	lcd_display_string(40 + 80, 160, (const uint8_t *)"y1", 16, RED);

	lcd_display_string(40, 180, (const uint8_t *)"x2", 16, RED);
	lcd_display_string(40 + 80, 180, (const uint8_t *)"y2", 16, RED);

	lcd_display_string(40, 200, (const uint8_t *)"x3", 16, RED);
	lcd_display_string(40 + 80, 200, (const uint8_t *)"y3", 16, RED);

	lcd_display_string(40, 220, (const uint8_t *)"x4", 16, RED);
	lcd_display_string(40 + 80, 220, (const uint8_t *)"y4", 16, RED);

	lcd_display_string(40, 240, (const uint8_t *)"fac is:", 16, RED);

	lcd_display_num(40 + 24, 160, hwXpos0, 4, 16, RED);
	lcd_display_num(40 + 24 + 80, 160, hwYpos0, 4, 16, RED);

	lcd_display_num(40 + 24, 180, hwXpos1, 4, 16, RED);
	lcd_display_num(40 + 24 + 80, 180, hwYpos1, 4, 16, RED);

	lcd_display_num(40 + 24, 200, hwXpos2, 4, 16, RED);
	lcd_display_num(40 + 24 + 80, 200, hwYpos2, 4, 16, RED);

	lcd_display_num(40 + 24, 220, hwXpos3, 4, 16, RED);
	lcd_display_num(40 + 24 + 80, 220, hwYpos3, 4, 16, RED);

	lcd_display_num(40 + 56, 240, hwFac, 3, 16, RED);
}

uint8_t tp_scan(uint8_t chCoordType)
{
	__SPI_128div;
	if (!(__XPT2046_IRQ_READ())) {
		if (chCoordType) {
			xpt2046_twice_read_xy(&s_tTouch.hwXpos, &s_tTouch.hwYpos);
		} else if (xpt2046_twice_read_xy(&s_tTouch.hwXpos, &s_tTouch.hwYpos)) {
			//s_tTouch.hwXpos = 0.066 * s_tTouch.hwXpos + (-12);//s_tTouch.fXfac * s_tTouch.hwXpos + s_tTouch.iXoff;
			//s_tTouch.hwYpos = (-0.089) * s_tTouch.hwYpos + (352);//s_tTouch.fYfac * s_tTouch.hwYpos + s_tTouch.iYoff;
			s_tTouch.hwXpos = s_tTouch.fXfac * s_tTouch.hwXpos + s_tTouch.iXoff;
			s_tTouch.hwYpos = s_tTouch.fYfac * s_tTouch.hwYpos + s_tTouch.iYoff;
		}
		if (0 == (s_tTouch.chStatus & TP_PRESS_DOWN)) {
			s_tTouch.chStatus = TP_PRESS_DOWN | TP_PRESSED;
			s_tTouch.hwXpos0 = s_tTouch.hwXpos;
			s_tTouch.hwYpos0 = s_tTouch.hwYpos;
		} 

	} else {
		if (s_tTouch.chStatus & TP_PRESS_DOWN) {
			s_tTouch.chStatus &= ~(1 << 7);
		} else {
			s_tTouch.hwXpos0 = 0;
			s_tTouch.hwYpos0 = 0;
			s_tTouch.hwXpos = 0xffff;
			s_tTouch.hwYpos = 0xffff;
		}
	}
	__SPI_2div;
	return (s_tTouch.chStatus & TP_PRESS_DOWN);
	
}


void tp_adjust(void)
{	
	
	uint8_t  cnt = 0;
	uint16_t hwTimeout = 0, d1, d2, pos_temp[4][2];
	uint32_t tem1, tem2;
	float fac;				

	lcd_clear_screen(WHITE);
	lcd_display_string(40, 40, (const uint8_t *)"Please use the stylus click the cross on the screen. The cross will always move until the screen adjustment is completed.",
					16, RED);
	tp_draw_touch_point(20, 20, RED);
	s_tTouch.chStatus = 0;
	s_tTouch.fXfac = 0;
	while (1) {
		tp_scan(1);
		if((s_tTouch.chStatus & 0xC0) == TP_PRESSED) {	
			hwTimeout = 0;
			s_tTouch.chStatus &= ~(1 << 6);
						   			   
			pos_temp[cnt][0] = s_tTouch.hwXpos;
			pos_temp[cnt][1] = s_tTouch.hwYpos;
			cnt ++;	  
			switch(cnt) {			   
				case 1:						 
					tp_draw_touch_point(20, 20, WHITE);
					tp_draw_touch_point(LCD_WIDTH - 20, 20, RED);
					break;
				case 2:
					tp_draw_touch_point(LCD_WIDTH - 20, 20, WHITE);
					tp_draw_touch_point(20, LCD_HEIGHT - 20, RED);
					break;
				case 3:
					tp_draw_touch_point(20, LCD_HEIGHT - 20, WHITE);
					tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20, RED);
					break;
				case 4:	
					tem1=abs((int16_t)(pos_temp[0][0]-pos_temp[1][0]));//x1-x2
					tem2=abs((int16_t)(pos_temp[0][1]-pos_temp[1][1]));//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d1=sqrt(tem1);

					tem1=abs((int16_t)(pos_temp[2][0]-pos_temp[3][0]));//x3-x4
					tem2=abs((int16_t)(pos_temp[2][1]-pos_temp[3][1]));//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d2=sqrt(tem1);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0) {
						cnt=0;
 						tp_show_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);  
						HAL_Delay(1000);
						lcd_fill_rect(96, 240, 24, 16, WHITE);
						tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20, WHITE);
						tp_draw_touch_point(20, 20, RED);
						continue;
					}

					tem1=abs((int16_t)(pos_temp[0][0]-pos_temp[2][0]));//x1-x3
					tem2=abs((int16_t)(pos_temp[0][1]-pos_temp[2][1]));//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d1=sqrt(tem1);//

					tem1=abs((int16_t)(pos_temp[1][0]-pos_temp[3][0]));//x2-x4
					tem2=abs((int16_t)(pos_temp[1][1]-pos_temp[3][1]));//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d2=sqrt(tem1);//
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05) {
						cnt=0;
 						tp_show_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//??��o?��oy?Y   
						HAL_Delay(1000);
						lcd_fill_rect(96, 240, 24, 16, WHITE);
						tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20, WHITE);
						tp_draw_touch_point(20, 20, RED);
						continue;
					}//
								   
					tem1=abs((int16_t)(pos_temp[1][0]-pos_temp[2][0]));//x1-x3
					tem2=abs((int16_t)(pos_temp[1][1]-pos_temp[2][1]));//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d1=sqrt(tem1);//

					tem1=abs((int16_t)(pos_temp[0][0]-pos_temp[3][0]));//x2-x4
					tem2=abs((int16_t)(pos_temp[0][1]-pos_temp[3][1]));//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					tem1+=tem2;
					d2=sqrt(tem1);//
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05) {
						cnt=0;	
 						tp_show_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//??��o?��oy?Y   
						HAL_Delay(1000);
						lcd_fill_rect(96, 240, 24, 16, WHITE);
						tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20, WHITE);
						tp_draw_touch_point(20, 20, RED);
						continue;
					}

					s_tTouch.fXfac = (float)(LCD_WIDTH - 40) / (int16_t)(pos_temp[1][0] - pos_temp[0][0]);	
					s_tTouch.iXoff = (LCD_WIDTH - s_tTouch.fXfac * (pos_temp[1][0] + pos_temp[0][0])) / 2;

					s_tTouch.fYfac = (float)(LCD_HEIGHT - 40) / (int16_t)(pos_temp[2][1] - pos_temp[0][1]);	  
					s_tTouch.iYoff = (LCD_HEIGHT - s_tTouch.fYfac * (pos_temp[2][1] + pos_temp[0][1])) / 2;

					
					if(abs(s_tTouch.fXfac) > 2 || abs(s_tTouch.fYfac) > 2) {
						cnt=0;
 				    	tp_draw_touch_point(LCD_WIDTH - 20, LCD_HEIGHT - 20, WHITE);
						tp_draw_touch_point(20, 20, RED);								
						lcd_display_string(40, 26, (const uint8_t *)"TP Need readjust!", 16, RED);
						continue;
					}
					lcd_clear_screen(WHITE);
					lcd_display_string(35, 110, (const uint8_t *)"Touch Screen Adjust OK!", 16, BLUE);
					HAL_Delay(1000); 
 					lcd_clear_screen(WHITE);  
					return;				 
			}
		}
		HAL_Delay(10);
		if (++ hwTimeout >= 1000) {
			break;
		}
 	}
}

uint16_t color_set(uint8_t red, uint8_t green, uint8_t blue)
{
	uint16_t color = (((red/8)<< 11) | ((green/4)<<5) | (blue/8));
	return color;
}

void DRAW_color_set(uint16_t xpos, uint16_t ypos)
{
	for(uint8_t i=0; i<=239; i++)
	{
		lcd_draw_line(i,0,i,10,color_set(i,0,0));
		lcd_draw_line(i,10,i,20,color_set(0,i,0));
		lcd_draw_line(i,20,i,30,color_set(0,0,i));
	}
	
	if (ypos < 10) color|= color_set(xpos,0,0); 
	if (ypos < 20 && ypos > 10) color|= color_set(0,xpos,0); 
	if (ypos < 30 && ypos > 20) color|= color_set(0,0,xpos); 
	
	lcd_fill_rect(5,300,15,15,color);
	//lcd_fill_rect(0,0,50,16,color);
}

void tp_dialog()
{
	color=BLACK;
	lcd_clear_screen(WHITE);
	DRAW_color_set(0,0);
	lcd_draw_line(LCD_WIDTH-40,0,LCD_WIDTH,0,BLACK);
	lcd_display_string(200, 300, (const uint8_t *)"CLEAR", 16, BLUE);
}

void tp_draw_board(void)
{
	char str[20];
	tp_scan(0);
	if (s_tTouch.chStatus & TP_PRESS_DOWN) {
		if (s_tTouch.hwXpos < LCD_WIDTH && s_tTouch.hwYpos < LCD_HEIGHT) {
			if ( s_tTouch.hwYpos < 30) DRAW_color_set(s_tTouch.hwXpos, s_tTouch.hwYpos);
			
			if (s_tTouch.hwXpos > 200 && s_tTouch.hwYpos > 300) tp_dialog();
			 else {
				tp_draw_big_point(s_tTouch.hwXpos, s_tTouch.hwYpos, color);
			}
		}
	}
}


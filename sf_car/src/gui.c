#include <gui.h>

void lcd_setup()
{
	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(0);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
}

void gui_setup()
{
	//start
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(0, 136, 160, 136);
	BSP_LCD_DisplayStringAt(38, 191, "START", LEFT_MODE);
	//stop
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(0, 0, 160, 136);
	BSP_LCD_DisplayStringAt(45, 55, "STOP", LEFT_MODE);
	//right
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(160, 0, 160, 136);
	BSP_LCD_DisplayStringAt(197, 55, "RIGHT", LEFT_MODE);
	//left
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_FillRect(160, 136, 160, 136);
	BSP_LCD_DisplayStringAt(205, 191, "LEFT", LEFT_MODE);
	//neutral
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_FillRect(320, 136, 160, 136);
	BSP_LCD_DisplayStringAt(340, 191, "NEUTRAL", LEFT_MODE);
	//changing direction
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	BSP_LCD_FillRect(320, 0, 160, 136);
	BSP_LCD_DisplayStringAt(357, 55, "SHIFT", LEFT_MODE);
}

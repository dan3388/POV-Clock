#include "DLED.h"
#include "DRTC.h"
#include "UART.h"
#include "DADC.h"

//#include "font_clearsans.c"
DLED g_dled;
DRTC g_rtc;
DUART g_uart; 
DADC g_adc;

int main()
{
	g_dled.Init();
	g_rtc.Init();
	g_uart.Init();
	g_adc.Init();
	
	
	while (1)
	{
		int Hr_button_Mask = (1<<21); //PTE 21
			int Min_button_Mask = (1<<20); //PTE 20
			if (!((PTE->PDIR) &(Hr_button_Mask)))
			{
				g_rtc.updateHr();
			}
			if (!((PTE->PDIR) &(Min_button_Mask)))
			{
				g_rtc.updateMin();
			}; 
		g_dled.SetBrightness(g_adc.GetAmbientLight());
		g_dled.convert(g_rtc.GetSeconds(), g_adc.GetTemperature());
		g_uart.setBuffer(g_rtc.GetSeconds());
	}
}
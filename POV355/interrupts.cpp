// C-linkage interrupt-handler stubs to call the actual handlers inside C++ classes
#include <MKL16Z4.h>
#include "DLED.h"
#include "DRTC.h"
#include "UART.h"
#include "DADC.h"

extern "C" {
	void PIT_IRQHandler()
	{
		g_dled.clock();
		if (PIT->CHANNEL[0].TFLG == 1) 
		{
			PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK; // Clear interrupt flag
		}
		
	} // PIT_IRQHandler()
	
	void PORTC_PORTD_IRQHandler() //GPIO intrupt: turns on and off LED 19
	{
		int button1_MASK = 0xFFFFFFFD;  //Mask to issolate the one bit that corresponds to the button
		if ((PORTD->ISFR) &(~button1_MASK)) //Checks if the correct ISFR flag is checked
		{	 
			if ((PTD->PDIR) &(~button1_MASK)) //Checks staus of correct GPIOD pin, Button is pull up so if the pin is high the LED should not be turned on, if pin is low then LED should be on
			{
				int Hr_button_Mask = 0xffdfffff; //PTE 21
				int Min_button_Mask = 0xffefffff; //PTE 20
				if ((PTE->PDIR) &(~Hr_button_Mask))
				{
					g_rtc.updateHr();
				}
				if ((PTE->PDIR) &(~Min_button_Mask))
				{
					g_rtc.updateMin();
				}
				
			}
			else
			{
				
			}
		}
		PORTD->ISFR = (PORTD->ISFR & ~button1_MASK); // Resets ISFR flag
	} //void PORTD_IRQHandler()
	
	// UART Interrupt Handler
  void UART0_IRQHandler() 
	{
    uint8_t status = UART0->S1;
      
    if (status & UART_S1_TDRE_MASK) 
		{ 
			g_uart.TransmitHandler();
    }
      
    if (status & UART_S1_RDRF_MASK) 
		{ 
			g_uart.ReceiveHandler();
    }
       
    UART0->S1 = 0xFF; // Clear all flags
  }
}
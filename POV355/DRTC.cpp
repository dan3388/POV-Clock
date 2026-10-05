// ENGR 355 (Winter 2025) RTC driver
#include "DRTC.h"
#include <MKL16Z4.h>
#include <stdint.h>


DRTC::DRTC()
{
	offset_Hr = 0;
	offset_Min = 0;
}

//Non returning function that initializes all necesary registers for RTC
void DRTC::Init()
{
	//enable PortD and PortE
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
	
	//Set PTD 1 and PTE 20/21 to with pull up resistors
	PORTD->PCR[1] = (PORTD->PCR[1] &~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1);
	PORTE->PCR[20] = (PORTE->PCR[20] &~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1);
	PORTE->PCR[21] = (PORTE->PCR[21] &~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1);
		//Enabling pull up on SW1 SW2 SW4
	PORTD->PCR[1] = (PORTD->PCR[1] &~PORT_PCR_PE_MASK) | PORT_PCR_PE_MASK;
	PORTD->PCR[1] = (PORTD->PCR[1] &~PORT_PCR_PS_MASK) | PORT_PCR_PS_MASK;
	
	PORTE->PCR[20] = (PORTE->PCR[20] &~PORT_PCR_PE_MASK) | PORT_PCR_PE_MASK;
	PORTE->PCR[20] = (PORTE->PCR[20] &~PORT_PCR_PS_MASK) | PORT_PCR_PS_MASK;
	
	PORTE->PCR[21] = (PORTE->PCR[21] &~PORT_PCR_PE_MASK) | PORT_PCR_PE_MASK;
	PORTE->PCR[21] = (PORTE->PCR[21] &~PORT_PCR_PS_MASK) | PORT_PCR_PS_MASK;
	
	//PORTD 1 IRQC set up
	PORTD->PCR[1] = (PORTD->PCR[1] &~PORT_PCR_IRQC_MASK) | PORT_PCR_IRQC(0b1011);
	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
	
	
	//Set port C pin 1 for RTC and enable alt 1 for RTC in
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[1] = (PORTC->PCR[1] &~PORT_PCR_MUX_MASK)|PORT_PCR_MUX(1);
	
	//Enabling RTC in 
	SIM->SOPT1 |= (SIM->SOPT1 & ~SIM_SOPT1_OSC32KSEL_MASK) | SIM_SOPT1_OSC32KSEL(2);
	SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;
	
	//set RTC to 0
	RTC->TSR = 0;
	
	//start RTC by setting TCE but in RTC->SR
	RTC->SR |= RTC_SR_TCE_MASK;
}

// returning that returns the TSR value + offet times
uint32_t DRTC::GetSeconds()
{
	return RTC->TSR + offset_Hr + offset_Min;
}

void DRTC::updateHr()
{
	// Protection for rollover 
	if (offset_Hr > 86400)
	{
		offset_Hr = 0;
	}
	offset_Hr += 3600;
}

void DRTC::updateMin()
{
	// Protection for rollover
	if (offset_Min > 3600)
	{
		offset_Min = 0;
	}
	offset_Min += 60;
}

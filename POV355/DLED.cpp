#include "DLED.h"

extern const uint16_t font[];
extern const int16_t fontIndex[];


// Function to enable PIT and PORTD IRQn and enable IRQ in the NVIC
void DLED::enableInterrupts()
{
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	// Enabling PIT and enableIRQ on NVIC for PIT
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->CHANNEL[0].LDVAL = 0x440; //0x575;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);	
}

// Function to enable PWM called in Init()
void DLED::enablePWM() 
{
	// Enabling clocking for TPM 48MHz
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC_MASK & SIM_SOPT2_TPMSRC(1));
	SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;

	// Enables TPM1 in SCGC6 
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	// Set TPM1 MOD value and enable CnSC registers for PWM
	TPM1->MOD = 159;
	TPM1->CNT = 0;
	TPM1->CONTROLS[0].CnV = 10;
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK;
	
	// Start counter for TPM1 
	TPM1->SC |= TPM_SC_CMOD(1);
	
}


// Non returning function that enables Ports C and D and turns the pins for the LED lights to be enabled
void DLED::Init() 
{
	// Enable clocks for ports B, C, and D
  SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTB_MASK);
    
  // Configure MUX for port C pins 2-11
  for (int i = 2; i < 12; i++) {
      PORTC->PCR[i] = (PORTC->PCR[i] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1);
  }
    
  // Configure MUX for port D pins 2-7
  for (int i = 2; i < 8; i++) {
      PORTD->PCR[i] = (PORTD->PCR[i] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(1);
  }
    
  // Set pins 2-11 on port C, and 2-7 on port D as outputs
  PTC->PDDR |= 0xFFC; // Pins 2-11 (0b111111111100)
  PTD->PDDR |= 0xFC;  // Pins 2-7 (0b11111100)
    
  // Enable TPM1 and configure PORTB[0] as TPM1_CH0 instead of output
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
  PORTB->PCR[0] = (PORTB->PCR[0] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(3); // Set PORTB0 as TPM1_CH0
    
  // Calling function that takes care of enabling necessary interrupts in NVIC
  enableInterrupts();
  // Calling function that takes care of enabling PWM and necessary registers
  enablePWM();
	
	// Setting 0 for spaces in the currentDisplay array
	// This for loop will set 3 spaces 0 uint16_t values in the array in the currentDisplay array
	for (int i = 0; i < 3; i++)
	{
		currentDisplay[18+i] = 0;
		currentDisplay[39+i] = 0;
		currentDisplay[46+i] = 0;
		currentDisplay[67+i] = 0;
		currentDisplay[88+i] = 0;
		currentDisplay[91+i] = 0;
		currentDisplay[112+i] = 0;
		currentDisplay[133+i] = 0;
		currentDisplay[154+i] = 0;
		currentDisplay[167+i] = 0;
	}
	// This loop will copy the values for the colon charcter
	for (int i = 0; i < 4; i++)
	{
		currentDisplay[42+i] = font[Ind_col+i];
	}
	// This loop will copy the values for the degree character
	for (int i = 0; i < 10; i++)
	{
		currentDisplay[157+i] = font[Ind_deg+i];
	}
	// This loop will copy the values for the degree character
	for (int i = 0; i < 14; i++)
	{
		currentDisplay[170+i] = font[Ind_F+i];
	}
}

// Performs necessary bit operations to update PDOR on Ports C and D
// with the LED pattern.
void DLED::UpdatePOV(uint16_t pattern)
{
	// Clear the existing bits before updating with new pattern
  PTC->PCOR = 0xFFC; // Clear bits for Pins 2-11 
  PTD->PCOR = 0xFC;  // Clear bits for Pins 2-7 
	
  // Write new values based on pattern
  PTC->PSOR = (pattern & 0x03FF) << 2; 
	PTD->PSOR = (pattern & 0xFC00) >> 8; 
} 

void DLED::SetBrightness(int num)
{// 0% == 0, 100% == 60, 263% == 159
	TPM1->CONTROLS[0].CnV = (((num*3)/5));
}

//Function is called by the PIT IRQ Handler to display the value
void DLED::clock()
{
	UpdatePOV(currentDisplay[PIT_index]);
	if (PIT_index == ARRAY_LENGTH - 1)
	{

		if (PITcounter >= 50)
		{
			updateTime();
			PITcounter = 0;
			PIT_index = 0;
		}
		else
		{
			PITcounter++;
		}
		
		UpdatePOV(0);
	}
	else
	{
		PIT_index++;
	}
}

//Function to Update Time
void DLED::updateTime()
{
	// loop through hour_ten, hour_one, minute_ten, minute_one
	for (int i = 0; i < 4; i++)
	{
		int digit = time[i];
		int startIndex = location[i];  // Get the starting position from location array
    int fontIndex = digitIndices[digit];  // Get the corresponding font index 
		
		// Copy all the digits taking into consideration the font width
		for (int j = 0; j < fontWidth[digit]; j++)
		{
			currentDisplay[startIndex + j] = font[fontIndex + j];
		}
	}
	// loop through hundred_temp, ten_temp, one_temp
	for (int i = 0; i < 3; i++)
	{
		int digit = temp[i];
		int startIndex = location[TIME+i];
		int fontIndex = digitIndices[digit];
		
		for (int j = 0; j < fontWidth[digit]; j++)
		{
			currentDisplay[startIndex + j] = font[fontIndex + j];
		}
	}
}

void DLED::convert(uint32_t clock, int tempInt)
{
	// Time conversion 
  int totalM = clock / 60;
  int totalH = (totalM / 60) % 24;
  int remainderM = totalM % 60;

  time[0] = totalH / 10;
  time[1] = totalH % 10;
  time[2] = remainderM / 10;
  time[3] = remainderM % 10;
	
	// Temp Conversion
	temp[0] = tempInt / 100;
	temp[1] = (tempInt / 10) % 10;
	temp[2] = tempInt % 10;
}


DLED::DLED()
{
}
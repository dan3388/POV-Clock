// ENGR 355 (Winter 2025) ADC driver
#include "DADC.h"
#include <MKL16Z4.h>
#include <stdint.h>
#include "DLED.h"
#include <cstdio>

DADC::DADC()
{
}

// Thermistor Value table used for conversion
const uint16_t DADC::ThermistorValues[NUM_THERMISTOR_TABLE_ENTRIES] = {
  59525,
  59339,
  59149,
  58952,
  58752,
  58547,
  58337,
  58122,
  57903,
  57679,
  57449,
  57216,
  56977,
  56734,
  56484,
  56230,
  55971,
  55708,
  55440,
  55166,
  54886,
  54602,
  54313,
  54019,
  53720,
  53416,
  53107,
  52794,
  52475,
  52151,
  51822,
  51488,
  51151,
  50808,
  50462,
  50111,
  49755,
  49394,
  49030,
  48662,
  48288,
  47909,
  47526,
  47142,
  46757,
  46365,
  45967,
  45568,
  45168,
  44763,
  44354,
  43941,
  43526,
  43109,
  42691,
  42272,
  41852,
  41429,
  41000,
  40569,
  40138,
  39709,
  39282,
  38851,
  38415,
  37979,
  37544,
  37105,
  36665,
  36228,
  35795,
  35367,
  34938,
  34503,
  34063,
  33622,
  33192,
  32768,
  32339,
  31911,
  31485,
  31061,
  30639,
  30219,
  29801,
  29384,
  28970,
  28560,
  28152,
  27746,
  27343,
  26944,
  26546,
  26151,
  25761,
  25374,
  24988,
  24607,
  24231,
  23858,
  23486,
  23119,
  22757,
  22398,
  22042,
  21690,
  21344,
  21002,
  20663,
  20325,
  19993,
  19665,
  19342,
  19023,
  18708,
  18397,
  18090,
  17787,
  17487,
  17193
};



//Non returning function that initializes all necesary registers for ADC 
void DADC::Init()
{
	//Turning on PortB 2 and ADC0
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[2] = (PORTB->PCR[2] &~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(0);//Setting defalt function for ADC0
	PORTB->PCR[3] = (PORTB->PCR[3] &~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(0);
	//Setting up ADC
	ADC0->CFG1 |= ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE_MASK | ADC_CFG1_ADICLK_MASK;//Low Power mode, no extra clock divider, long ampling time, 16bit conversions, built-in async clock source
	ADC0->CFG2 &= ~(ADC_CFG2_ADACKEN_MASK | ADC_CFG2_ADHSC_MASK | ADC_CFG2_ADLSTS_MASK);//Async clock only during ADC conversions, disable high-speed mode, longgest sampling time (24 ADCK cycles total)
	ADC0->SC3 |= ADC_SC3_CAL_MASK | ADC_SC3_AVGE_MASK | ADC_SC3_AVGS_MASK;//Enable hardware averaging, 32 samples averaged
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)){}//calibration
	uint16_t tmp;
	// Calculate and store plus-side calibration
	tmp  = ADC0->CLP0;
	tmp += ADC0->CLP1;
	tmp += ADC0->CLP2;
	tmp += ADC0->CLP3;
	tmp += ADC0->CLP4;
	tmp += ADC0->CLPS;
	tmp = (tmp >>1) | 0x8000;
	ADC0->PG = tmp;
	// Calculate and store minus-side calibration
	tmp  = ADC0->CLM0;
	tmp += ADC0->CLM1;
	tmp += ADC0->CLM2;
	tmp += ADC0->CLM3;
	tmp += ADC0->CLM4;
	tmp += ADC0->CLMS;
	tmp = (tmp >>1) | 0x8000;
	ADC0->MG = tmp;	
	
}

// Int returning function to convert ADC values to temperature
int DADC::GetTemperature()
{
	ADC0->SC1[0] = ADC_SC1_ADCH(0b01100); //looking at PORTB 2 to get temperature
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)){}
	uint16_t temp = ADC0->R[0];
	//return temp; //for testing
	//g_pov.setLED(temp); //for testing
	
	for(int i=0;i <120;i++) //linear search for aprox temp, Binary was hard and would bassically end up bing linear after a certain point
	{
		if(temp > ThermistorValues[i])
		{
			return i;
		}
	}
	return 0;
}

// Int returing function to convert ADC values to light levels
int DADC::GetAmbientLight()
{
	ADC0->SC1[0] = ADC_SC1_ADCH(0b01101);//PORTB 3 to get anbient light
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)){}
	uint16_t light = ADC0->R[0];
		//low: 0x6D5e = 27998
		//middle: 0xA70E = 42766
		//high: 0xC423 = 50211
	light = (552*light)/100000 -143;
	return light;
		
}
#ifndef DADC_H
#define DADC_H
#include <stdint.h>

class DADC
{
public:
  
  DADC();
    
  // Public API
  void Init();
	int GetTemperature();
	int GetAmbientLight();
	
	

private:
  DADC(const DADC&);
  void operator=(const DADC&);
	enum
     {
       NUM_THERMISTOR_TABLE_ENTRIES = 120,
       ADC_CHANNEL_THERMISTOR = 12,
       ADC_CHANNEL_LIGHT = 13,
     };
	static const uint16_t ThermistorValues[NUM_THERMISTOR_TABLE_ENTRIES];
	int result;
};

// Every user of the GADC driver class will get this when they include DADC.h.
// It tells the linker that there is an instance of DADC called g_adc---the one and
// only instance of the GADC driver, typically instantiated at the top of main.cpp.
//
// This means you must use "g_ADC" as the name of your global DADC instance... or
// change both this name and the global variable definition in main.cpp.
extern DADC g_adc;

#endif  // DADC_H

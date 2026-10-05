#include "MKL16Z4.h"
#include <stdint.h>

#ifndef DLED_H
#define DLED_H

class DLED
{
	public:
		DLED();
		void Init();
		void UpdatePOV(uint16_t pattern);
		void SetBrightness(int num);
		void clock();
		void updateTime();
		void convert(uint32_t clock, int tempInt);
		
	private:
		// Enum to store indexes for all the different arrays used in the Driver Class
		enum 
		{
			TEMP = 3,
			TIME = 4,
			ARRAY_LENGTH = 184,
			Ind_0 = 148,
			Ind_1 = 166,
			Ind_2 = 176, 
			Ind_3 = 193,
			Ind_4 = 210,
			Ind_5 = 229,
			Ind_6 = 247,
			Ind_7 = 264,
			Ind_8 = 281,
			Ind_9 = 298,
			Ind_col = 315,
			Ind_deg = 1362,
			Ind_F = 517,
			ten_h = 0,
			ten_m = 43,
			one_h = 21,
			one_m = 70,
			hundred_temp = 99,
			ten_temp = 117,
			one_temp = 136
		};
		
		const int fontWidth[10]={18, 10, 17, 17, 19, 18, 17, 17, 17, 17};
		const int digitIndices[10]={ Ind_0, Ind_1, Ind_2, Ind_3, Ind_4, Ind_5, Ind_6, Ind_7, Ind_8, Ind_9 };
		const int location[7] = {ten_h, one_h, ten_m, one_m, hundred_temp, ten_temp, one_temp};
		
		 
		int time[TIME];
		int temp[TEMP];
		int PIT_index;
		int PITcounter = 0;
		uint16_t currentDisplay[ARRAY_LENGTH]; 
		DLED(const DLED&);
		void enableInterrupts();
		void enablePWM();
		void operator=(const DLED&);
};

// Every user of the DLED driver class will get this when they include DLED.h.
// It tells the linker that there is an instance of DLED called g_dled---the one and
// only instance of the GPIO driver, typically instantiated at the top of main.cpp.
//
// This means you must use "g_dled" as the name of your global DLED instance... or
// change both this name and the global variable definition in main.cpp.
extern DLED g_dled;

#endif  // DLED_H
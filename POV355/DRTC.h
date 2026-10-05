#ifndef DRTC_H
#define DRTC_H
#include <stdint.h>

class DRTC
{
public:
  
  DRTC();
    
  // Public API
  void Init();
	uint32_t GetSeconds();
	void updateHr();
	void updateMin();
	
private:
  DRTC(const DRTC&);
  void operator=(const DRTC&);
	int realTime;
	int offset_Hr;
	int offset_Min;
	
};

// Every user of the GRTC driver class will get this when they include DRTC.h.
// It tells the linker that there is an instance of DRTC called g_rtc---the one and
// only instance of the GRTC driver, typically instantiated at the top of main.cpp.
//
// This means you must use "g_rtc" as the name of your global DRTC instance... or
// change both this name and the global variable definition in main.cpp.
extern DRTC g_rtc;

#endif  // DRTC_H

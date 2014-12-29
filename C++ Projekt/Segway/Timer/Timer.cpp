#include "Timer.h"

// Adressen
#define TC 0xFFFF3800 //Timer
#define CCR0_Offset 0x00 //Control Register
#define CMR0_Offset 0x04 //Mode Register
#define CV0_Offset 0x10 //Counter Value
#define RA0_Offset 0x14 //Register A
#define RB0_Offset 0x18 //Register B
#define RC0_Offset 0x1C //Register C
#define SR0_Offset 0x20 //Status Register
#define IER0_Offset 0x24 //Interrupt Enable Register
#define IDR0_Offset 0x28 //Interrupt Disable Register
#define IMR0_Offset 0x2C //Interrupt Mask Register

Timer::Timer() {

}

Timer::~Timer() {

}

// RC value will determinate
bool Timer::prepareTimer(unsigned long frequency){
	VINTP CMR0 = (int*) (TC+CMR0_Offset);
	VINTP RC0 = (int*) (TC+RC0_Offset);
	setIsTimerInterruptEnabled(true);
	SET_BIT(*CMR0,15); //*CMR0 |= 1 << 15; // WAVE: Waveform mode is enabled
	SET_BIT(*CMR0,14); //*CMR0 |= 1 << 14; // WAVSEL: RC is chosen as the maximum value (UP mode with automatic trigger on RC Compare)
	SET_BIT(*CMR0,1); //*CMR0 |= 1 << 1; // TCCLKS: TIMER_CLOCK3 is chosen, the clock choise is define in Configuration.cpp
	if (2000000/frequency <= 65535){  // > 2^16 - 1 -> There are only 16 bits availabe
		*RC0 = 2000000/frequency; // frequency will hand over // One Interrupt per second happens when RC=2 MHz, if we want 5 Interrupts (f=5 Hz) we have to divide 2 MHz with 5
		return true;	
	}
	else if (2000000/frequency > 65535){
		return false;
	}
}
void Timer::cleanUpTimer(void){
	/* Diese Methode wird nicht aufgerufen
	*CCR0 = 0x0;
	*CMR0 = 0x0;
	*CV0 = 0x0;
	*RA0 = 0x0;
	*RB0 = 0x0;
	*RC0 = 0x0;
	*SR0 = 0x0;
	*IER0 = 0x0;
	*IDR0 = 0x0;
	*IMR0 =0x0;*/
}

bool Timer::initTimer( unsigned long frequency ) {
	if (prepareTimer(frequency) == true){
		setIsTimerInterruptEnabled(true);
		return true;
	}
	else {
		return false;	
	}
}

void Timer::setIsTimerEnabled( bool enabled ) {
	VINTP CCR0 = (int*) (TC+CCR0_Offset);
	if (enabled == true){
		SET_BIT(*CCR0, 0); //Entpricht: *CCR0 |= 1; CLKEN: This bit will enable the clock 
		SET_BIT(*CCR0, 2); //Entspricht: *CCR0 |= 1 << 2; SWTRG: The clock is started
	}
	else{
		SET_BIT(*CCR0, 1); //Entspricht: *CCR0 |= 1 << 1; CLKDIS: This bit will disable the clock
	}
	}

void Timer::setIsTimerInterruptEnabled( bool enabled ) {
	VINTP IER0 = (int*) (TC+IER0_Offset);
	VINTP IDR0 = (int*) (TC+IDR0_Offset);
	if (enabled == true){
		SET_BIT(*IER0, 4); //Entspricht *IER0 |= 1 << 4; // CPCS:This bit is set when an RC Compare has occured 
	}
	else{
		SET_BIT(*IDR0, 4); //Entspricht: *IDR0 |= 1 << 4; //CPCS: This bit ill clear the sorresponding bit in IMR (Interrupt Mask Register, read-only)
	}
	}

void Timer::resetInterruptFlag(void) {
	VINTP SR0 = (int*) (TC+SR0_Offset);
    BIT_IS_SET(*SR0, 4); //Reading the Status Register will also clear the interrupt bit for the corresponding interrupts.
}


//If timer is enabled, the return value is true
bool Timer::getIsTimerEnabled(void){
	VINTP SR0 = (int*) (TC+SR0_Offset);
	return (bool)(*SR0 & 0x10000); // Bit 16 CLKSTA is set when the clock is enabled -> must be 1 (2^16 = 16^4)
}

//If interrupts are enabled, the return value is true
bool Timer::getIsInterruptEnabled(void){
	VINTP IMR0 = (int*) (TC+IMR0_Offset);
	return (bool)(*IMR0 & 0x10); // Bit 4 CPCS is set when an RC Compare has occured (2^4 = 16^1)
}

void Timer::cleanUp(void){
	/* Diese Methode wird nicht aufgerufen
	*CCR0 = 0x0;
	*CMR0 = 0x0;
	*CV0 = 0x0;
	*RA0 = 0x0;
	*RB0 = 0x0;
	*RC0 = 0x0;
	*SR0 = 0x0;
	*IER0 = 0x0;
	*IDR0 = 0x0;
	*IMR0 =0x0;
	*/
}

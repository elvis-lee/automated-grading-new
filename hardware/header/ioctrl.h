#ifndef _IOCTRL_H_
#define _IOCTRL_H_

// ----- INCLUDES -----
#include <stdint.h>

// ----- PUBLIC DEFS -----
#define IO_SUCCESS (0)
#define IO_BUFFULL (1)

// ----- PUBLIC STRUCTS -----
typedef struct{
	uint32_t t;
	uint16_t regvals;
} ioevent_digital_t;

typedef struct{
	uint32_t t;
	uint16_t channel;
	uint16_t analogval;
} ioevent_analog_t;

// ----- PUBLIC API -----
int io_pushDigitalOut(uint32_t t, uint16_t regvals);
int io_pushAnalogOut(uint32_t t, uint16_t channel, uint16_t dacval);
ioevent_digital_t* io_peakNextDigitalOut();
ioevent_analog_t* io_peakNextAnalogOut();
ioevent_digital_t io_popNextDigitalOut();
ioevent_analog_t io_popNextAnalogOut();
uint16_t io_getNumDigitalOut();
uint16_t io_getNumAnalogOut();

#endif // _IOCTRL_H_
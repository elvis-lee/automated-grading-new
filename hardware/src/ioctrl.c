// ----- INCLUDES -----
#include "ioctrl.h"

// ----- PRIVATE DEFINITIONS -----
#define BUFSIZE_DIGITAL (1000)
#define BUFSIZE_ANALOG (1000)

// ----- PRIVATE STRUCTS -----
typedef struct{
	uint16_t head;
	uint16_t tail;
	uint16_t size;
	ioevent_digital_t data[BUFSIZE_DIGITAL];
} DigitalQueue_t;

typedef struct{
	uint16_t head;
	uint16_t tail;
	uint16_t size;
	ioevent_analog_t data[BUFSIZE_ANALOG];
} AnalogQueue_t;

// ----- PRIVATE VARIABLES -----
// digital ring buffers
DigitalQueue_t bufout_digital = {.head=0, .tail=0, .size=0};
DigitalQueue_t bufin_digital = {.head=0, .tail=0, .size=0};
// analog ring buffers
AnalogQueue_t bufout_analog = {.head=0, .tail=0, .size=0};
AnalogQueue_t bufin_analog = {.head=0, .tail=0, .size=0};

// ----- PUBLIC FUNCTION DEFINITIONS -----
int io_pushDigitalOut(uint32_t t, uint16_t regvals)
{
	ioevent_digital_t event = {.t=t, .regvals=regvals};
	if( bufout_digital.size < BUFSIZE_DIGITAL )
	{
		bufout_digital.data[bufout_digital.head] = event;
		bufout_digital.head = (bufout_digital.head+1) % BUFSIZE_DIGITAL;
		bufout_digital.size++;
	}
	else
	{
		bufout_digital.data[bufout_digital.head] = event;
		bufout_digital.head = (bufout_digital.head+1) % BUFSIZE_DIGITAL;
		bufout_digital.tail = (bufout_digital.tail+1) % BUFSIZE_DIGITAL;
	}
}

int io_pushAnalogOut(uint32_t t, uint16_t channel, uint16_t dacval)
{
	ioevent_analog_t event = {.t=t, .channel=channel, .analogval=dacval};
	if( bufout_analog.size < BUFSIZE_ANALOG )
	{
		bufout_analog.data[bufout_analog.head] = event;
		bufout_analog.head = (bufout_analog.head+1) % BUFSIZE_ANALOG;
		bufout_analog.size++;
	}
	else
	{
		bufout_analog.data[bufout_analog.head] = event;
		bufout_analog.head = (bufout_analog.head+1) % BUFSIZE_ANALOG;
		bufout_analog.tail = (bufout_analog.tail+1) % BUFSIZE_ANALOG;
	}
}

ioevent_digital_t* io_peakNextDigitalOut()
{
	return &bufout_digital.data[bufout_digital.tail];
}

ioevent_analog_t* io_peakNextAnalogOut()
{
	return &bufout_analog.data[bufout_analog.tail];
}

void io_popNextDigitalOut()
{
	ioevent_digital_t event = bufout_digital.data[bufout_digital.tail];
	bufout_digital.tail = (bufout_digital.tail+1) % BUFSIZE_DIGITAL;
	bufout_digital.size--;
}

void io_popNextAnalogOut()
{
	ioevent_analog_t event = bufout_analog.data[bufout_analog.tail];
	bufout_analog.tail = (bufout_analog.tail+1) % BUFSIZE_ANALOG;
	bufout_analog.size--;
}

uint16_t io_getNumDigitalOut()
{
	return bufout_digital.size;
}

uint16_t io_getNumAnalogOut()
{
	return bufout_analog.size;
}



// ----- PRIVATE FUNCTION DEFINITIONS -----

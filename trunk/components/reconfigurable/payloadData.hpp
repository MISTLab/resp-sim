#ifndef PAYLOADDATA_HPP
#define PAYLOADDATA_HPP

#include <systemc.h>

typedef struct {
	unsigned int address;
	sc_time latency;
	unsigned int width;
	unsigned int height;
	unsigned int answer;
} payloadData;

#endif

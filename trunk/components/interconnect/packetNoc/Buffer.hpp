#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "nocCommon.hpp"

class Buffer{
private:
	unsigned int max_buffer_size;
	queue<Packet> buffer;

public:
	Buffer();
	~Buffer();

	void setMaxBufferSize(unsigned int max);
	unsigned int getMaxBufferSize();
	unsigned int getCurrentFreeSlots();
	unsigned int size();

	bool isFull();
	bool isEmpty();

	void drop(Packet& p);		// called by push when buffer is full
	void empty();			// called by pop when buffer is empty

	bool push(Packet& p);
	Packet pop();
	Packet front();			// return a copy of the first element of the buffer
};

#endif

#include "Buffer.hpp"

Buffer::Buffer() {
	setMaxBufferSize(DEFAULT_BUFFER_SIZE);
}

Buffer::~Buffer() {}

void Buffer::setMaxBufferSize(unsigned int max) {
	max_buffer_size = max;
}

unsigned int Buffer::getMaxBufferSize() {
	return max_buffer_size;
}

unsigned int Buffer::size() {
	return buffer.size();
}

unsigned int Buffer::getCurrentFreeSlots() {
	return(getMaxBufferSize()-buffer.size());
}
bool Buffer::isFull() {
	return buffer.size() == max_buffer_size;
}

bool Buffer::isEmpty() {
	return buffer.size() == 0;
}

void Buffer::drop(Packet& p) {
	// Do nothing...
}

void Buffer::empty() {
	// Do nothing...
}

bool Buffer::push(Packet& p) {
	if(isFull()){
		drop(p);
		return false;
	} else {
		buffer.push(p);
		return true;
	}
}

Packet Buffer::pop() {
	Packet p;
	if(isEmpty()){
		empty();
		return Packet();
	} else {
		p = buffer.front();
		buffer.pop();
	}
	return p;
}

Packet Buffer::front() {
	Packet p;
	if(isEmpty()){
		empty();
		return Packet();
	} else {
		p = buffer.front();
	}
	return p;
}



/**
 * eFPGA Allocation Table: a structure containing the data for the management of
 * the hardware functions mapped to a single reconfigurable device
 */

#ifndef  _EFPGATABLE_H
#define  _EFPGATABLE_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <systemc.h>

using namespace std;

// A single cell in the eFPGA (essentially a matrix)
struct gridPosition {
	unsigned int row;
	unsigned int column;
};

// A block of cells, represented by its bounds
struct block {
	gridPosition upperLeft;
	gridPosition lowerRight;
};

class eFPGAAllocationTable {

private:
	string name;

	/*
	 * The following tables contain data identifying the hardware functions in the device.
	 * The first element of each map is called 'key' of the function, and corresponds
	 * to an 'address' in the general Configuration Engine Table.
	 */
	map<unsigned int, sc_time> latTable;
	map<unsigned int, block> occupationTable;

	// This is the list of the free blocks in the eFPGA
	list<block> freeSpace;

	/*
	 * Searches in the free space the best postion for a 'width' X 'height' cells large function.
	 * Returns an integer heuristic (the lower the better) measuring the quality of the fit in
	 * the block returned at address 'position'; this heuristic should be the best one that can
	 * be obtained on the fabric; if the allocation is not feasible, (unsigned int)-1 is returned.
	 * FUNCTION IMPLEMENTED IN FILE allocationFit.cpp
	 */
	unsigned int allocationFit(unsigned int width, unsigned int height, block* position);

public:

	// Constructor
	eFPGAAllocationTable(string n, unsigned int width, unsigned int height);

	/*
	 * Maps the function 'key' and its 'latency' in the position assigned by the allocationFit method;
	 * 'width' and 'height' are used as parameters for allocationFit.
	 * Returns FALSE if there is not enough free space or some error happens, TRUE otherwise.
	 */
	bool add(unsigned int key, sc_time latency, unsigned int width, unsigned int height);

	/*
	 * Removes the function addressed by 'key' and frees the space.
	 * Returns FALSE if 'key' doesn't exists or some error happens, TRUE otherwise.
	 */
	bool remove(unsigned int key);

	/*
	 * "Executes" the function addressed by 'key' and writes its latency at the address 'foundLatency'.
	 * Returns FALSE if 'key' doesn't exists or some error happens, TRUE otherwise.
	 */
	bool execute(unsigned int key, sc_time* foundLatency);

	/*
	 * Searches for the best heuristic using the allocationFit method, fed with 'width' and 'height'.
	 * Returns the heuristic (the lower the better); (unsigned int)-1 if not feasible.
	 */
	unsigned int searchForSpace(unsigned int width, unsigned int height);

	/*
	 * Prints on the standard output the internal status of the device table.
	 */
	void printStatus();

};

#endif  //_EFPGATABLE_H

/**
 * Configuration Engine Allocation Table: a structure containing the data for the management of
 * the hardware functions mapped on the enitre reconfigurable system
 */

#ifndef  _CETABLE_H
#define  _CETABLE_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <cstdlib>
#include <ctime>

using namespace std;

// Enumerating the implemented function deletion algorithms
enum deletionAlgorithm {
	// Deletes the oldest function configured in the system
	FIFO,
	// Deletes the oldest function executed in the system; functions marked 0 are never executed;
	// if more functions are marked 0, the one with the lowest address is deleted
	LRU,
	RANDOM	
};

class cEAllocationTable {

private:
	/*
	 * The following tables contain data identifying the hardware functions in the system.
	 * The first element of each map is called 'address' of the function.
	 * The maps start addressing at 1. Address 0 is reserved to signal errors.
	 * Even the devices start with 1. Device 0 is reserved to signal errors.
	 */
	map<unsigned int, string> nameTable;
	map<unsigned int, unsigned int> deviceTable;
	map<unsigned int, unsigned int> deletionData;
	unsigned int ctr;

	// Identifies the algorithm to be used for function deletion
	deletionAlgorithm delType;
	unsigned int delUtil;

public:
	// Constructor
	cEAllocationTable(deletionAlgorithm delAlg);

	/*
	 * Maps the function 'name' and its corresponding 'device'; if needed, updates the deletion algorithm data.
	 * Returns the address at which the function is mapped. If the table is full, device is equal to 0, name is
	 * empty or some other error happens, reserved address 0 is returned.
	 */
	unsigned int add(string name, unsigned int device);

	/*
	 * States that the function mapped at 'address' is executed; if needed, updates the deletion algorithm data.
	 * Returns FALSE if the address has no mapping, TRUE otherwise.
	 */
	bool exec(unsigned int address);

	/*
	 * Removes a function according to the choosen deletion algorithm (or the given 'address'/'name').
	 * Returns the address of the function which is removed, and puts its device number at address 'devnum'.
	 * If no functions are mapped, or some other error happens, reserved address 0 is returned.
	 */
	unsigned int remove(unsigned int *devnum);
	unsigned int remove(unsigned int address, unsigned int *devnum);
	unsigned int remove(string name, unsigned int *devnum);

	/*
	 * Returns the name of the function mapped at 'address'.
	 * If the address has no mapping, an empty string is returned.
	 */
	string getName(unsigned int address);

	/*
	 * Returns the number of the device implementing the function mapped at 'address'.
	 * If the address has no mapping, reserved device 0 is returned.
	 */
	unsigned int getDevice(unsigned int address);

	/*
	 * Returns the address of the function named 'name' (if various instances are
	 * configured, the one with the lower ranking in deletion algorithm is returned).
	 * If the name has no mapping, reserved address 0 is returned.
	 */
	unsigned int getAddress(string name);

	/*
	 * Prints on the standard output the internal status of the general table.
	 */
	void printStatus();

};

#endif  //_CETABLE_H

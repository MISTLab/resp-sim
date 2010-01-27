#include "eFPGAAllocationTable.hpp"

// Utility function: merges the confining blocks into a single block
list<block> merge(list<block> separated) {
	list<block>::iterator sepIt = separated.begin(), sepSubIt;
	list<block> merged;
	block newBlock;
	bool check;

	while ( sepIt!=separated.end() ) {
		sepSubIt = merged.begin();
		check = false;
		while ( sepSubIt != merged.end() ) {
			if (	(*sepIt).upperLeft.row == (*sepSubIt).lowerRight.row + 1 &&
				(*sepIt).upperLeft.column == (*sepSubIt).upperLeft.column &&
				(*sepIt).lowerRight.column == (*sepSubIt).lowerRight.column ) {
					newBlock.upperLeft.row=(*sepSubIt).upperLeft.row;
					newBlock.upperLeft.column=(*sepIt).upperLeft.column;
					newBlock.lowerRight.row=(*sepIt).lowerRight.row;
					newBlock.lowerRight.column=(*sepIt).lowerRight.column;
					check = true;
			}
			if (	(*sepIt).upperLeft.column == (*sepSubIt).lowerRight.column + 1 &&
				(*sepIt).upperLeft.row == (*sepSubIt).upperLeft.row &&
				(*sepIt).lowerRight.row == (*sepSubIt).lowerRight.row ) {
					newBlock.upperLeft.row=(*sepIt).upperLeft.row;
					newBlock.upperLeft.column=(*sepSubIt).upperLeft.column;
					newBlock.lowerRight.row=(*sepIt).lowerRight.row;
					newBlock.lowerRight.column=(*sepIt).lowerRight.column;
					check = true;
			}
			if (	(*sepIt).lowerRight.row == (*sepSubIt).upperLeft.row - 1 &&
				(*sepIt).upperLeft.column == (*sepSubIt).upperLeft.column &&
				(*sepIt).lowerRight.column == (*sepSubIt).lowerRight.column ) {
					newBlock.upperLeft.row=(*sepIt).upperLeft.row;
					newBlock.upperLeft.column=(*sepIt).upperLeft.column;
					newBlock.lowerRight.row=(*sepSubIt).lowerRight.row;
					newBlock.lowerRight.column=(*sepIt).lowerRight.column;
					check = true;
			}
			if (	(*sepIt).lowerRight.column == (*sepSubIt).upperLeft.column - 1 &&
				(*sepIt).upperLeft.row == (*sepSubIt).upperLeft.row &&
				(*sepIt).lowerRight.row == (*sepSubIt).lowerRight.row ) {
					newBlock.upperLeft.row=(*sepIt).upperLeft.row;
					newBlock.upperLeft.column=(*sepIt).upperLeft.column;
					newBlock.lowerRight.row=(*sepIt).lowerRight.row;
					newBlock.lowerRight.column=(*sepSubIt).lowerRight.column;
					check = true;
			}
			if (check) {
				merged.insert(merged.end(),newBlock);
				merged.erase(sepSubIt);
				break;
			}
			sepSubIt++;
		}
		if (!check) merged.insert(merged.end(),(*sepIt));
		sepIt++;
	}
	if (merged.size() == separated.size()) return merged;
	return (merge(merged));
}

eFPGAAllocationTable::eFPGAAllocationTable(string n, unsigned int width, unsigned int height) {
	name=n;
	block totalSpace;
	totalSpace.upperLeft.row=1;
	totalSpace.upperLeft.column=1;
	totalSpace.lowerRight.row=height;
	totalSpace.lowerRight.column=width;
	freeSpace.insert(freeSpace.end(),totalSpace);
}

bool eFPGAAllocationTable::add(unsigned int key, sc_time latency, unsigned int width, unsigned int height) {

	// Checking if the key is already assigned
	map<unsigned int, sc_time>::iterator exists = latTable.find(key);
	if ( exists != latTable.end() ) return false;

	// Obtaining the best position for the function (if exists)
	block selectedBlock;
	if ( allocationFit(width,height,&selectedBlock)==(unsigned int)-1 ) return false;

	// Mapping the new function
	block occupiedBlock;
	occupiedBlock.upperLeft.row=selectedBlock.upperLeft.row;
	occupiedBlock.upperLeft.column=selectedBlock.upperLeft.column;
	occupiedBlock.lowerRight.row=selectedBlock.upperLeft.row+height-1;
	occupiedBlock.lowerRight.column=selectedBlock.upperLeft.column+width-1;
	latTable[key]=latency;
	occupationTable[key]=occupiedBlock;

	// Erasing/Modifying all the free blocks subscribed by the new function
	block freeBlock;
	list<block> newFreeSpace;
	list<block>::iterator searchFree = freeSpace.begin();
	list<block>::iterator toErase;
	bool upper,lower,left;
	while (searchFree != freeSpace.end() ) {
		upper = false;
		lower = false;
		left = false;
		if (	(*searchFree).upperLeft.row > occupiedBlock.lowerRight.row ||
			(*searchFree).upperLeft.column > occupiedBlock.lowerRight.column ||
			(*searchFree).lowerRight.row < occupiedBlock.upperLeft.row ||
			(*searchFree).lowerRight.column < occupiedBlock.upperLeft.column ) {
					newFreeSpace.insert(newFreeSpace.end(), (*searchFree) );
		}
		else {
			if ( (*searchFree).upperLeft.row < occupiedBlock.upperLeft.row ) {
				freeBlock.upperLeft.row=(*searchFree).upperLeft.row;
				freeBlock.upperLeft.column=(*searchFree).upperLeft.column;
				freeBlock.lowerRight.row=occupiedBlock.upperLeft.row-1;
				freeBlock.lowerRight.column=(*searchFree).lowerRight.column;
				newFreeSpace.insert(newFreeSpace.end(), freeBlock);
				upper = true;
			}
			if ( (*searchFree).upperLeft.column < occupiedBlock.upperLeft.column) {
				if (upper) freeBlock.upperLeft.row=occupiedBlock.upperLeft.row;
				else freeBlock.upperLeft.row=(*searchFree).upperLeft.row;
				freeBlock.upperLeft.column=(*searchFree).upperLeft.column;
				freeBlock.lowerRight.row=(*searchFree).lowerRight.row;
				freeBlock.lowerRight.column=occupiedBlock.upperLeft.column-1;
				newFreeSpace.insert(newFreeSpace.end(), freeBlock);
				left=true;
			}
			if ( (*searchFree).lowerRight.row > occupiedBlock.lowerRight.row ) {
				freeBlock.upperLeft.row=occupiedBlock.lowerRight.row+1;
				if (left) freeBlock.upperLeft.column=occupiedBlock.upperLeft.column;
				else freeBlock.upperLeft.column=(*searchFree).upperLeft.column;
				freeBlock.lowerRight.row=(*searchFree).lowerRight.row;
				freeBlock.lowerRight.column=(*searchFree).lowerRight.column;
				newFreeSpace.insert(newFreeSpace.end(), freeBlock);
				lower = true;
			}
			if ( (*searchFree).lowerRight.column > occupiedBlock.lowerRight.column) {
				if (upper) freeBlock.upperLeft.row=occupiedBlock.upperLeft.row;
				else  freeBlock.upperLeft.row=(*searchFree).upperLeft.row;
				freeBlock.upperLeft.column=occupiedBlock.lowerRight.column+1;
				if (lower) freeBlock.lowerRight.row=occupiedBlock.lowerRight.row;
				else freeBlock.lowerRight.row=(*searchFree).lowerRight.row;
				freeBlock.lowerRight.column=(*searchFree).lowerRight.column;
				newFreeSpace.insert(newFreeSpace.end(), freeBlock);
			}
		}	
		searchFree++;
	}

	freeSpace = merge(newFreeSpace);

//	cout << name << ": added function " << key << " with parameter " << latTable[key] << endl;
	return true;
}

bool eFPGAAllocationTable::remove(unsigned int key) {
	map<unsigned int, block>::iterator exists = occupationTable.find(key);
	if (exists == occupationTable.end()) {
		cerr << name << ": the function marked " << key << " wasn't found on this fabric: something wrong happened!" << endl;
		return false;
	}
	
	block freed = exists->second;
	freeSpace.insert(freeSpace.end(),freed);

	latTable.erase(key);
	occupationTable.erase(key);

	freeSpace=merge(freeSpace);

//	cerr << name << ": removed function marked " << key << endl;
	return true;
}

bool eFPGAAllocationTable::execute(unsigned int key, sc_time* foundLatency) {
	map<unsigned int, sc_time>::iterator exists = latTable.find(key);
	if (exists == latTable.end()) {
		cerr << name << ": the function marked " << key << " wasn't found on this fabric: something wrong happened!" << endl;
		*foundLatency = SC_ZERO_TIME;
		return false;
	}
//	cerr << name << ": function at address " << key << " executed!" << endl;
	*foundLatency = exists->second;//sc_time(exists->second, SC_NS);

	return true;
}

unsigned int eFPGAAllocationTable::searchForSpace(unsigned int width, unsigned int height) {
	block tmp;
	return allocationFit(width,height,&tmp);
}

void eFPGAAllocationTable::printStatus() {
	map<unsigned int, sc_time>::iterator latIt = latTable.begin();
	map<unsigned int, block>::iterator bloIt;
	list<block>::iterator searchFree;
	unsigned int key;

	cout << endl << "\tDEVICE " << name << ":" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	cout << "|    Key     |       Latency        |                     Position                     |" << endl;
	cout << "----------------------------------------------------------------------------------------" << endl;
	while ( latIt != latTable.end() ) {
		key = latIt->first;
		bloIt = occupationTable.find(key);
		cout << setiosflags (ios::left);
		cout << "| " << setw (10) << key << " | " << setw (17) << latIt->second << " ns | ";
		cout << "{" << setw (10) << (bloIt->second).upperLeft.row << "," << setw (10) << (bloIt->second).upperLeft.column << "}; ";
		cout << "{" << setw (10) << (bloIt->second).lowerRight.row << "," << setw (10) << (bloIt->second).lowerRight.column << "} |"<< endl;
		latIt++;
	}
	cout << "----------------------------------------------------------------------------------------" << endl;
	cout << "|                   Free Blocks                    |" << endl;
	cout << "----------------------------------------------------" << endl;
	for (searchFree = freeSpace.begin(); searchFree!=freeSpace.end(); searchFree++) {
		cout << "| {" << setw (10) << (*searchFree).upperLeft.row << "," << setw (10) << (*searchFree).upperLeft.column << "}; ";
		cout << "{" << setw (10) << (*searchFree).lowerRight.row << "," << setw (10) << (*searchFree).lowerRight.column << "} |"<< endl;
	}
	cout << "----------------------------------------------------" << endl;
}


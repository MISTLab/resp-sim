#include "eFPGAAllocationTable.hpp"

// Utility function: calculates a list of maximum expansion blocks
list<block> max_blocks(list<block> chopped) {
	list<block>::iterator chopIt = chopped.begin(), chopSubIt, tmp;
	list<block> blended;
	block newBlock;
	bool check,recursive=false,oldrec;
	//printf("in ");

	while ( chopIt!=chopped.end() ) {
		blended.insert(blended.end(),(*chopIt));
		//printf("0 ");
		chopSubIt = blended.begin();
		check = false;
		while ( chopSubIt!=blended.end() ) {
			if ( (*chopIt).upperLeft.row == (*chopSubIt).lowerRight.row + 1 ) {
				if (	(*chopIt).upperLeft.column <= (*chopSubIt).upperLeft.column &&
					(*chopIt).lowerRight.column >= (*chopSubIt).upperLeft.column ) {
						newBlock.upperLeft.row=(*chopSubIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopSubIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopIt).lowerRight.column;
						if ( (*chopSubIt).lowerRight.column < newBlock.lowerRight.column )
							newBlock.lowerRight.column=(*chopSubIt).lowerRight.column;
						//printf("1 ");
						check = true;
				}
				else if((*chopIt).upperLeft.column <= (*chopSubIt).lowerRight.column &&
					(*chopIt).lowerRight.column >= (*chopSubIt).lowerRight.column ) {
						newBlock.upperLeft.row=(*chopSubIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopSubIt).lowerRight.column;
						//printf("2 ");
						check = true;
				}
				else if((*chopIt).upperLeft.column > (*chopSubIt).upperLeft.column &&
					(*chopIt).lowerRight.column < (*chopSubIt).lowerRight.column ) {
						newBlock.upperLeft.row=(*chopSubIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopIt).lowerRight.column;
						//printf("s ");
						check = true;
				}
			}
			if ( (*chopIt).upperLeft.column == (*chopSubIt).lowerRight.column + 1 ) {
				if (	(*chopIt).upperLeft.row <= (*chopSubIt).upperLeft.row &&
					(*chopIt).lowerRight.row >= (*chopSubIt).upperLeft.row ) {
						newBlock.upperLeft.row=(*chopSubIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopSubIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopIt).lowerRight.row;
						if ( (*chopSubIt).lowerRight.row < newBlock.lowerRight.row )
							newBlock.lowerRight.row=(*chopSubIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopIt).lowerRight.column;
						//printf("3 ");
						check = true;
				}
				else if((*chopIt).upperLeft.row <= (*chopSubIt).lowerRight.row &&
					(*chopIt).lowerRight.row >= (*chopSubIt).lowerRight.row ) {
						newBlock.upperLeft.row=(*chopIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopSubIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopSubIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopIt).lowerRight.column;
						//printf("4 ");
						check = true;
				}
				else if((*chopIt).upperLeft.row > (*chopSubIt).upperLeft.row &&
					(*chopIt).lowerRight.row < (*chopSubIt).lowerRight.row ) {
						newBlock.upperLeft.row=(*chopIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopSubIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopIt).lowerRight.column;
						//printf("t ");
						check = true;
				}
			}
			if ( (*chopIt).lowerRight.row == (*chopSubIt).upperLeft.row - 1 ) {
				if (	(*chopIt).upperLeft.column <= (*chopSubIt).upperLeft.column &&
					(*chopIt).lowerRight.column >= (*chopSubIt).upperLeft.column ) {
						newBlock.upperLeft.row=(*chopIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopSubIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopSubIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopIt).lowerRight.column;
						if ( (*chopSubIt).lowerRight.column < newBlock.lowerRight.column )
							newBlock.lowerRight.column=(*chopSubIt).lowerRight.column;
						//printf("5 ");
						check = true;
				}
				else if((*chopIt).upperLeft.column <= (*chopSubIt).lowerRight.column &&
					(*chopIt).lowerRight.column >= (*chopSubIt).lowerRight.column ) {
						newBlock.upperLeft.row=(*chopIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopSubIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopSubIt).lowerRight.column;
						//printf("6 ");
						check = true;
				}
				else if((*chopIt).upperLeft.column > (*chopSubIt).upperLeft.column &&
					(*chopIt).lowerRight.column < (*chopSubIt).lowerRight.column ) {
						newBlock.upperLeft.row=(*chopIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopSubIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopIt).lowerRight.column;
						//printf("x ");
						check = true;
				}
			}
			if ( (*chopIt).lowerRight.column == (*chopSubIt).upperLeft.column - 1 ) {
				if (	(*chopIt).upperLeft.row <= (*chopSubIt).upperLeft.row &&
					(*chopIt).lowerRight.row >= (*chopSubIt).upperLeft.row ) {
						newBlock.upperLeft.row=(*chopSubIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopIt).lowerRight.row;
						if ( (*chopSubIt).lowerRight.row < newBlock.lowerRight.row )
							newBlock.lowerRight.row=(*chopSubIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopSubIt).lowerRight.column;
						//printf("7 ");
						check = true;
				}
				else if ((*chopIt).upperLeft.row <= (*chopSubIt).lowerRight.row &&
					(*chopIt).lowerRight.row >= (*chopSubIt).lowerRight.row ) {
						newBlock.upperLeft.row=(*chopIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopSubIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopSubIt).lowerRight.column;
						//printf("8 ");
						check = true;
				}
				else if((*chopIt).upperLeft.row > (*chopSubIt).upperLeft.row &&
					(*chopIt).lowerRight.row < (*chopSubIt).lowerRight.row ) {
						newBlock.upperLeft.row=(*chopIt).upperLeft.row;
						newBlock.upperLeft.column=(*chopIt).upperLeft.column;
						newBlock.lowerRight.row=(*chopIt).lowerRight.row;
						newBlock.lowerRight.column=(*chopSubIt).lowerRight.column;
						//printf("y ");
						check = true;
				}
			}
			if (check) {
				oldrec = recursive;
				recursive = true;
				for (tmp = blended.begin(); tmp != blended.end(); tmp++)
					if (	newBlock.upperLeft.row >= (*tmp).upperLeft.row &&
						newBlock.upperLeft.column >=(*tmp).upperLeft.column &&
						newBlock.lowerRight.row <= (*tmp).lowerRight.row &&
						newBlock.lowerRight.column <= (*tmp).lowerRight.column )
							{recursive = oldrec; }//printf("h ");}
				for (tmp = chopped.begin(); tmp != chopped.end(); tmp++)
					if (	newBlock.upperLeft.row >= (*tmp).upperLeft.row &&
						newBlock.upperLeft.column >=(*tmp).upperLeft.column &&
						newBlock.lowerRight.row <= (*tmp).lowerRight.row &&
						newBlock.lowerRight.column <= (*tmp).lowerRight.column )
							{recursive = oldrec; }//printf("j ");}
				blended.insert(blended.end(),newBlock);
				check = false;
			}
			chopSubIt++;
		}
		chopIt++;
	}

	bool isMax;
	for (chopIt = blended.begin(); chopIt != blended.end(); ) {
		isMax = true;
		for (chopSubIt = blended.begin(); chopSubIt != blended.end(); chopSubIt++)
			if (	chopIt != chopSubIt &&
				(*chopIt).upperLeft.row >= (*chopSubIt).upperLeft.row &&
				(*chopIt).upperLeft.column >= (*chopSubIt).upperLeft.column &&
				(*chopIt).lowerRight.row <= (*chopSubIt).lowerRight.row &&
				(*chopIt).lowerRight.column <= (*chopSubIt).lowerRight.column )
					isMax = false;
			// Blocchi uguali contenuti l'uno nell'altro!!!
		if (isMax) chopIt++;
		else {
			tmp = chopIt;
			chopIt++;
			blended.erase(tmp);
		}
	}

	if(recursive) return max_blocks(blended);
	return blended;
}

// Heuristic is: number of free cells left in a block by the allocation
unsigned int eFPGAAllocationTable::allocationFit(unsigned int width, unsigned int height, block* position) {

	list<block> maxFree = max_blocks(freeSpace);
	list<block>::iterator searchFree = maxFree.begin();

	// Checking if there are free blocks
	if  ( searchFree == maxFree.end() ) return (unsigned int)-1;

	// Loops searching for the block closer to 'width' X 'height'
	unsigned int spWidth, spHeight, count, min = (unsigned int) -1;
	do {
		spWidth = ( (*searchFree).lowerRight.column - (*searchFree).upperLeft.column + 1 );
		spHeight = ( (*searchFree).lowerRight.row - (*searchFree).upperLeft.row + 1 );
		if (width<=spWidth && height<=spHeight) {
			count = (spWidth * spHeight) - (width * height);
			if (count <= min) {
				min = count;
				(*position).upperLeft.row = (*searchFree).upperLeft.row;
				(*position).upperLeft.column = (*searchFree).upperLeft.column;
				(*position).lowerRight.row = (*searchFree).lowerRight.row;
				(*position).lowerRight.column = (*searchFree).lowerRight.column;
			}
		}
		searchFree++;
	} while ( searchFree != maxFree.end() );

	if ( (int)min == -1 ) {
		(*position).upperLeft.row = 0;
		(*position).upperLeft.column = 0;
		(*position).lowerRight.row = 0;
		(*position).lowerRight.column = 0;
	}

	return min;
}


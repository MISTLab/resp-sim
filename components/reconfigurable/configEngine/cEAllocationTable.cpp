/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   The following code is derived, directly or indirectly, from the SystemC
 *   source code Copyright (c) 1996-2004 by all Contributors.
 *   All Rights reserved.
 *
 *   The contents of this file are subject to the restrictions and limitations
 *   set forth in the SystemC Open Source License Version 2.4 (the "License");
 *   You may not use this file except in compliance with such restrictions and
 *   limitations. You may obtain instructions on how to receive a copy of the
 *   License at http://www.systemc.org/. Software distributed by Contributors
 *   under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 *   ANY KIND, either express or implied. See the License for the specific
 *   language governing rights and limitations under the License.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
 *   Component developed by: Fabio Arlati arlati.fabio@gmail.com
 *
\***************************************************************************/

#include "cEAllocationTable.hpp"

cEAllocationTable::cEAllocationTable(deletionAlgorithm delAlg) {
	srand(static_cast<int>(time(NULL)));
	delType=delAlg;
	ctr=1;
	delUtil=1;
}

unsigned int cEAllocationTable::add(string name, unsigned int device) {

	// Parameters check
	if (device==0 || name.length()==0) return 0;

	/*
	 * NOTE: the following two checks are useful only if ctr already
	 * completed a loop in the unsigned int range (not so probable:) );
	 */

	// Since 0 is reserved, ctr should be incremented to one
	if (ctr==0) {ctr++;}
	// Checks for ctr, if the actual value overwrites an older function
	map<unsigned int, string>::iterator nameIt = nameTable.find(ctr);
	unsigned int ctrold=ctr;
	while (nameIt!=nameTable.end()) {
		ctr++;
		if (ctr==0) {ctr++;}
		if (ctr==ctrold) return 0;	// A loop is completed, the table is full!!
		nameIt = nameTable.find(ctr);
	}

	nameTable[ctr]=name;
	deviceTable[ctr]=device;
	if (delType==FIFO) {
		deletionData[ctr]=delUtil;
		delUtil++;
		if (delUtil==0) {
			/*
			 * If the deletion counter is back to 0, a loop in the unsigned int
			 * range is completed. The data is resorted...
			 */
			unsigned int tmpDelCtr, min, whereMin, size = deletionData.size();
			map<unsigned int, unsigned int> tempDel=deletionData;
			map<unsigned int, unsigned int>::iterator delIt;
			for (tmpDelCtr=1; tmpDelCtr<(size+1); tmpDelCtr++) {
				delIt = tempDel.begin();
				whereMin = delIt->first;
				min = delIt->second;
				while (delIt != tempDel.end()) {
					if (delIt->second < min) {
						whereMin = delIt->first;
						min = delIt->second;
					}
					delIt++;
				}
				deletionData[whereMin] = tmpDelCtr;
				tempDel.erase(whereMin);
			}
			delUtil=tmpDelCtr;
		}
	}
	if (delType==LRU || delType == RANDOM) deletionData[ctr]=0;
	return ctr++;

}

bool cEAllocationTable::exec(unsigned int address) {
	map<unsigned int, unsigned int>::iterator exists = deletionData.find(address);
	if (exists == deletionData.end()) return false;
	if (delType == LRU) {
		deletionData[address]=delUtil;
		delUtil++;
		if (delUtil==0) {
			/*
			 * If the deletion counter is back to 0, a loop in the unsigned int
			 * range is completed. The data is resorted...
			 */
			unsigned int tmpDelCtr, min, whereMin, size;
			map<unsigned int, unsigned int> tempDel=deletionData;
			map<unsigned int, unsigned int>::iterator delIt;
			delIt = deletionData.begin();
			while (delIt != deletionData.end()) {
				if (delIt->second == 0) {
					tempDel.erase(delIt->first);
				}
				delIt++;
			}
			size = tempDel.size();
			for (tmpDelCtr=1; tmpDelCtr<(size+1); tmpDelCtr++) {
				delIt = tempDel.begin();
				whereMin = delIt->first;
				min = delIt->second;
				while (delIt != tempDel.end()) {
					if (delIt->second < min) {
						whereMin = delIt->first;
						min = delIt->second;
					}
					delIt++;
				}
				deletionData[whereMin] = tmpDelCtr;
				tempDel.erase(whereMin);
			}
			delUtil=tmpDelCtr;
		}
	}
		
	return true;
}

unsigned int cEAllocationTable::remove(unsigned int *devnum) {
	if ( delType == FIFO || delType == LRU) {
		unsigned int min, whereMin;
		map<unsigned int, unsigned int>::iterator delIt;
		delIt = deletionData.begin();
		// No functions mapped!!
		if (delIt == deletionData.end() ) {
			(*devnum) = 0;
			return 0;
		}

		whereMin = delIt->first;
		min = delIt->second;
		while (delIt != deletionData.end()) {
			if (delIt->second < min) {
				whereMin = delIt->first;
				min = delIt->second;
			}
			delIt++;
		}
		nameTable.erase(whereMin);
		(*devnum)=deviceTable[whereMin];
		deviceTable.erase(whereMin);
		deletionData.erase(whereMin);
		return whereMin;
	}
	if ( delType == RANDOM ) {
		unsigned int whereMin,i;
		map<unsigned int, unsigned int>::iterator delIt;
		delIt = deletionData.begin();
		// No functions mapped!!
		if (delIt == deletionData.end() ) {
			(*devnum) = 0;
			return 0;
		}
		whereMin = rand() % nameTable.size();
		for (i = 0; i < whereMin; i++ ) {
			delIt++;
		}
		whereMin = delIt->first;
		nameTable.erase(whereMin);
		(*devnum)=deviceTable[whereMin];
		deviceTable.erase(whereMin);
		deletionData.erase(whereMin);
		return whereMin;
	}		
}

unsigned int cEAllocationTable::remove(unsigned int address, unsigned int *devnum) {
	map<unsigned int, string>::iterator exists = nameTable.find(address);
	if (exists == nameTable.end()) {
		(*devnum) = 0;
		return 0;
	}
	else {
		nameTable.erase(address);
		(*devnum)=deviceTable[address];
		deviceTable.erase(address);
		deletionData.erase(address);
		return address;
	}
}

unsigned int cEAllocationTable::remove(string name, unsigned int *devnum) {
	unsigned int address = getAddress(name);
	if (address == 0) {
		(*devnum) = 0;
		return 0;
	}
	nameTable.erase(address);
	(*devnum)=deviceTable[address];
	deviceTable.erase(address);
	deletionData.erase(address);
	return address;
}

string cEAllocationTable::getName(unsigned int address) {
	map<unsigned int, string>::iterator exists = nameTable.find(address);
	if (exists == nameTable.end()) return "";
	else return exists->second;
}

unsigned int cEAllocationTable::getDevice(unsigned int address) {
	map<unsigned int, unsigned int>::iterator exists = deviceTable.find(address);
	if (exists == deviceTable.end()) return 0;
	else return exists->second;
}

unsigned int cEAllocationTable::getAddress(string name) {
	map<unsigned int, string>::iterator exists = nameTable.begin();
	map<unsigned int, unsigned int>::iterator dataIt;
	unsigned int myKey, min=0, whereMin=0;

	while (exists != nameTable.end()) {
		if ( name.compare(exists->second) == 0 ) {
			myKey = exists->first;
			dataIt = deletionData.find(myKey);
			if (dataIt == deletionData.end()) {
				cerr << "Something wrong happened!" << endl;
				return 0;
			}
			min = dataIt->second;
			whereMin=myKey;
			break;
		}
		exists++;
	}

	while (exists != nameTable.end()) {
		if ( name.compare(exists->second) == 0 ) {
			myKey = exists->first;
			dataIt = deletionData.find(myKey);
			if (dataIt == deletionData.end()) {
				cerr << "Something wrong happened!" << endl;
				return 0;
			}
			if (dataIt->second < min) {
				min = dataIt->second;
				whereMin=myKey;
			}
		}
		exists++;
	}

	return whereMin;
}

void cEAllocationTable::printStatus() {
	map<unsigned int, string>::iterator nameIt = nameTable.begin();
	map<unsigned int, unsigned int>::iterator devIt;
	map<unsigned int, unsigned int>::iterator delIt;
	unsigned int key;
	string tmpName;

	cout << endl << "\tENGINE TABLE" << endl;
	cout << "-------------------------------------------------------------------------" << endl;
	if (delType==FIFO) cout << "|  Address   |              Name              |  Device #  |  Inserted  |" << endl;
	if (delType==LRU) cout << "|  Address   |              Name              |  Device #  |  Executed  |" << endl;
	if (delType==RANDOM) cout << "|  Address   |              Name              |  Device #  |     //     |" << endl;
	cout << "-------------------------------------------------------------------------" << endl;
	while ( nameIt != nameTable.end() ) {
		key = nameIt->first;
		devIt = deviceTable.find(key);
		delIt = deletionData.find(key);
		tmpName = nameIt->second;
		if (tmpName.length() > 30) tmpName = tmpName.substr(0,27) + "...";
		cout << setiosflags (ios::left);
		cout << "| " << setw (10) << key << " | " << setw (30) << tmpName << " | ";
		cout << setw (10) << devIt->second << " | " << setw (10) << delIt->second << " |"<< endl;
		nameIt++;
	}
	cout << "-------------------------------------------------------------------------" << endl;
}


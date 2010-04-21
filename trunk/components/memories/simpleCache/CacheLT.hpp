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
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
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

#ifndef CACHELT_HPP
#define CACHELT_HPP

//#define DEBUGMODE

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <fstream>

#include "utils.hpp"

using namespace std;
using namespace tlm;
using namespace tlm_utils;

enum removePolicyType {LRU = 0, FIFO, RANDOM, NONE};
enum writePolicyType {THROUGH = 0, THROUGH_ALL, BACK};
struct CacheBlock {
	sc_dt::uint64 base_address;
	bool dirty;
	unsigned char* block;
	CacheBlock() {
		base_address = 0;
		dirty = false;
		block = NULL;
	}
};

template<typename BUSWIDTH> class CacheLT: public sc_module {
private:
	// Scratchpad Memory
	bool scratchpadEn;
	sc_time scratchLatency;
	sc_dt::uint64 scratchSize;
	sc_dt::uint64 scratchStart;
	unsigned char *scratchMemory;

	// The limit of the address which can be cached; if we go out from this
	// address then the request just goes through the cache (this is made
	// in order not to cache requests directed to memory mapped peripherals)
	sc_dt::uint64 cacheLimit;

	// Tag calculator and cache policies
	#define GET_TAG(address) ( (address >> shiftQuant) )
	removePolicyType removePolicy;
	writePolicyType writePolicy;

	// Cache size (in bytes) and basic partitioning
	sc_dt::uint64 size;
	unsigned int bytesPerWord;
	unsigned int wordsPerBlock;
	unsigned int numWays;

	// Derived cache sizes and measures
	unsigned int blockSize;
	unsigned int positions;
	unsigned int sets;

	// Size in bits of the different address areas
	unsigned int displacement;
	unsigned int setDispl;
	unsigned int shiftQuant;

	// Cache Latencies: each one of them is referred to a single block
	sc_time cacheReadLatency;
	sc_time cacheWriteLatency;
	sc_time cacheLoadLatency;
	sc_time cacheStoreLatency;
	sc_time cacheRemoveLatency;

	// Tha actual cache memory
	map< sc_dt::uint64, deque<CacheBlock> > cache;

	CacheBlock loadCacheBlock(sc_dt::uint64 address) {
		CacheBlock nB;
		tlm_generic_payload message;
		sc_time memLatency = SC_ZERO_TIME;
		nB.base_address = address - address % blockSize;
		nB.block = (unsigned char*) malloc (blockSize*sizeof(unsigned char));

		message.set_data_length(blockSize);
		message.set_data_ptr(nB.block);
		message.set_read();
		message.set_address(nB.base_address);
		message.set_response_status(TLM_INCOMPLETE_RESPONSE);
		this->initSocket->b_transport(message,memLatency);
		if (message.get_response_status() != TLM_OK_RESPONSE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error while reading from main memory");

		this->numReadBusAcc++;
		return nB;
	}

	void storeCacheBlock(CacheBlock cB) {
		tlm_generic_payload message;
		sc_time memLatency = SC_ZERO_TIME;
		if (cB.block == NULL)
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Something wrong happened in the cache, a block cannot be stored since it has no data");
		message.set_data_length(blockSize);
		message.set_data_ptr(cB.block);
		message.set_write();
		message.set_address(cB.base_address);
		message.set_response_status(TLM_INCOMPLETE_RESPONSE);
		this->initSocket->b_transport(message,memLatency);
		if (message.get_response_status() != TLM_OK_RESPONSE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error while writing to main memory");

		this->numWriteBusAcc++;
	}

	void removeCacheBlock(sc_dt::uint64 tag) {
		CacheBlock evicted;
		if (removePolicy == RANDOM) {
			// I have to randomly chose an element to be replaced
			unsigned int toReplace = rand()%this->cache[tag].size();
			deque<CacheBlock>::iterator blockIter = this->cache[tag].begin();
			for (unsigned int i=0; i<toReplace; i++) blockIter++;
			evicted = *(blockIter);
			this->cache[tag].erase(blockIter);
		}
		else {
			// LRU and FIFO will always remove the last element in queue. Order is controlled by the read (or write) routine
			evicted = this->cache[tag].back();
			this->cache[tag].pop_back();
		}

		// Write-Back should write the block in memory if it has been modified
		if (this->writePolicy == BACK && evicted.dirty) {storeCacheBlock(evicted); wait(this->cacheStoreLatency);}
	}

public:
	simple_target_socket<CacheLT, sizeof(BUSWIDTH)*8> targetSocket;
	simple_initiator_socket<CacheLT, sizeof(BUSWIDTH)*8> initSocket;

	// Statistics
	unsigned int numReadMiss;
	unsigned int numReadHit;
	unsigned int numWriteMiss;
	unsigned int numWriteHit;
	unsigned int numScratchAcc;

	unsigned int numAccesses;
	unsigned int numReadBusAcc;
	unsigned int numWriteBusAcc;

	CacheLT(sc_module_name module_name, sc_dt::uint64 size, sc_dt::uint64 limit, unsigned int wordsPerBlock, unsigned int numWays, removePolicyType rP = LRU, writePolicyType wP = BACK,
		sc_time readLatency = SC_ZERO_TIME, sc_time writeLatency = SC_ZERO_TIME, sc_time loadLatency = SC_ZERO_TIME, sc_time storeLatency = SC_ZERO_TIME, sc_time removeLatency = SC_ZERO_TIME) :
			sc_module(module_name), size(size), cacheLimit(limit),
			wordsPerBlock(wordsPerBlock), numWays(numWays),removePolicy(rP),writePolicy(wP),
			cacheReadLatency(readLatency), cacheWriteLatency(writeLatency), cacheLoadLatency(loadLatency),
			cacheStoreLatency(storeLatency), cacheRemoveLatency(removeLatency),
			targetSocket((boost::lexical_cast<std::string>(module_name) + "_targSock").c_str()),
			initSocket((boost::lexical_cast<std::string>(module_name) + "_initSock").c_str()) {

		this->targetSocket.register_b_transport(this, &CacheLT::b_transport);
		this->targetSocket.register_get_direct_mem_ptr(this, &CacheLT::get_direct_mem_ptr);
		this->targetSocket.register_transport_dbg(this, &CacheLT::transport_dbg);

		this->scratchpadEn = false;
		this->numScratchAcc = 0;
		this->scratchMemory = NULL;

		srand((unsigned)time(0));
		this->numReadMiss = 0;
		this->numReadHit = 0;
		this->numWriteMiss = 0;
		this->numWriteHit = 0;
		this->numAccesses = 0;
		this->numReadBusAcc = 0;
		this->numWriteBusAcc = 0;

		this->bytesPerWord = sizeof(BUSWIDTH);
		this->blockSize = bytesPerWord * wordsPerBlock;
		this->positions = size / blockSize;
		this->sets = positions / numWays;
		/*
		 * If numWays == positions --> sets == 1 --> Associative Cache
		 * If numWays == 1 --> sets == positions --> Direct Mapped Cache
		 */
		this->displacement = (float) (log(blockSize)/log(2));
		this->setDispl = (float) (log(sets)/log(2));
		this->shiftQuant = displacement + setDispl;

		end_module();
	}

	~CacheLT(){
		if (scratchMemory!=NULL) delete this->scratchMemory;
	}

	void setScratchpad(sc_dt::uint64 scratchStart, sc_dt::uint64 scratchSize, sc_time latency = SC_ZERO_TIME) {
		this->scratchpadEn = true;
		this->scratchLatency = latency;

		// Checking if scratchpad start address is lower than memory mapped peripherals
		if(scratchStart > cacheLimit){
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error in the value of parameter scratchStart: " << scratchStart << " It should be lower than " << cacheLimit);
		}
		// Checking if scratchpad start address is a power of 2
		unsigned int numOnes = 0;
		for(unsigned int i = 0; i < sizeof(int)*8; i++) {
			if((scratchStart & (0x1 << i)) != 0) numOnes++;
		}
		if(numOnes != 1){
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error in the value of parameter scratchStart: " << scratchStart << " It should be a power of 2");
		}
		this->scratchStart = scratchStart;

		// Checking if scratchpad size is a power of 2
		numOnes = 0;
		for(unsigned int i = 0; i < sizeof(int)*8; i++) {
			if((scratchSize & (0x1 << i)) != 0) numOnes++;
		}
		if(numOnes != 1){
			THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error in the value of parameter scratchSize: " << scratchSize << " It should be a power of 2");
		}
		this->scratchSize = scratchSize;

		if(this->scratchMemory != NULL){
			delete [] this->scratchMemory;
			this->scratchMemory = NULL;
		}
		this->scratchMemory = new unsigned char[this->scratchSize];
		memset(this->scratchMemory, 0, scratchSize);
	}

	void setReadLatency(sc_time latency) {this->cacheReadLatency = latency;}
	void setWriteLatency(sc_time latency) {this->cacheWriteLatency = latency;}
	void setLoadLatency(sc_time latency) {this->cacheLoadLatency = latency;}
	void setStoreLatency(sc_time latency) {this->cacheStoreLatency = latency;}
	void setRemoveLatency(sc_time latency) {this->cacheRemoveLatency = latency;}

	void readFromCache(sc_dt::uint64 address, unsigned char *data, unsigned int dataLen) {
		// Calculating the TAG associated to the required memory position
		sc_dt::uint64 tag = GET_TAG(address);

		// Declaring some support variables...


		deque<CacheBlock>::iterator tagIter;
		bool hit;
		CacheBlock curBlock;
		sc_dt::uint64 curBaseAddress;
		int cachePointerModifier;
		unsigned int partLen;
		unsigned int remLen = dataLen;
		unsigned char *dataPointer = data;

		// For each block containing part of the requested data...
		for (curBaseAddress = address - (address % this->blockSize); curBaseAddress < address+dataLen; curBaseAddress += this->blockSize) {

			hit = false;
			// For each block cached...
			// N.B. If no blocks are currently cached, cache[tag] is an ampty queue!
			for (tagIter = this->cache[tag].begin(); tagIter != this->cache[tag].end(); tagIter++) {
				// If the required block is cached...
				if (tagIter->base_address == curBaseAddress) {
					// We have a HIT!
					#ifdef DEBUGMODE
					cerr << sc_time_stamp() << ": " << name() << " - Read Hit! @" << curBaseAddress << endl;
					#endif
					hit = true;
					this->numReadHit++;

					// We save the current block...
					curBlock = *(tagIter);
					// And we upgrade the queue order if we use an LRU policy
					if (this->removePolicy==LRU) {
						this->cache[tag].erase(tagIter);
						this->cache[tag].push_front(curBlock);
					}
					break;
				}
			}
			// If we didn't hit...
			if (!hit) {
				// We have a MISS...
				#ifdef DEBUGMODE
				cerr << sc_time_stamp() << ": " << name() << " - Read Miss! @" << curBaseAddress << endl;
				#endif			
				this->numReadMiss++;
				// We load the block from memory...
				curBlock = this->loadCacheBlock(curBaseAddress);
				wait(this->cacheLoadLatency);

				// And we add it to the cache (after checking that it isn't already full, or worse...)
				if (this->cache[tag].size() == this->numWays) {this->removeCacheBlock(tag); wait(this->cacheRemoveLatency);}
				else if (this->cache[tag].size() > this->numWays)
					THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Something wrong happened in the cache, too many blocks under the same tag");
				this->cache[tag].push_front(curBlock);
			}

			// Finally, we can actually read the required data from the block...

			// We calculate the displacement of the data in the cache block...
			cachePointerModifier = address - curBlock.base_address;
			if (cachePointerModifier < 0) cachePointerModifier = 0;
			// And the length of the data to be read
			partLen = remLen;
			if (partLen + cachePointerModifier > this->blockSize) partLen = this->blockSize - cachePointerModifier;
			// We perform the effective read...
			memcpy(dataPointer, curBlock.block + cachePointerModifier, partLen*sizeof(unsigned char));
			wait(this->cacheReadLatency);
			// And we update the pointer to the required data for eventual subsequent reads
			remLen -= partLen;
			dataPointer += partLen;
		}
	}

	void writeToCache(sc_dt::uint64 address, unsigned char *data, unsigned int dataLen) {
		// Calculating the TAG associated to the required memory position
		sc_dt::uint64 tag = GET_TAG(address);

		// Declaring some support variables...
		deque<CacheBlock>::iterator tagIter;
		bool hit;
		CacheBlock curBlock;
		sc_dt::uint64 curBaseAddress;
		int cachePointerModifier;
		unsigned int partLen;
		unsigned int remLen = dataLen;
		unsigned char *dataPointer = data;

		// For each block containing part of the target data...
		for (curBaseAddress = address - (address % this->blockSize); curBaseAddress < address+dataLen; curBaseAddress += this->blockSize) {

			hit = false;
			// For each block cached...
			// N.B. If no blocks are currently cached, cache[tag] is an ampty queue!
			for (tagIter = this->cache[tag].begin(); tagIter != this->cache[tag].end(); tagIter++) {
				// If the required block is cached...
				if (tagIter->base_address == curBaseAddress) {
					// We have a HIT!
					#ifdef DEBUGMODE
					cerr << sc_time_stamp() << ": " << name() << " - Write Hit! @" << curBaseAddress << endl;
					#endif			
					hit = true;
					this->numWriteHit++;

					// We save the current block...
					tagIter->dirty = true;
					curBlock = *(tagIter);
					// And we upgrade the queue order if we use an LRU policy
					if (this->removePolicy==LRU) {
						this->cache[tag].erase(tagIter);
						this->cache[tag].push_front(curBlock);
					}
					break;
				}
			}
			// If we didn't hit...
			if (!hit) {
				// We have a MISS...
				#ifdef DEBUGMODE
				cerr << sc_time_stamp() << ": " << name() <<  " - Write Miss! @" << curBaseAddress << endl;
				#endif			
				this->numWriteMiss++;
				// If we have a Write-Back or Write-Through-All policy...
				if (this->writePolicy == BACK || this->writePolicy == THROUGH_ALL) {
					// We load the block from memory...
					curBlock = this->loadCacheBlock(curBaseAddress);
					curBlock.dirty = true;
					wait(this->cacheLoadLatency);

					// And we add it to the cache (after checking that it isn't already full, or worse...)
					if (this->cache[tag].size() == this->numWays) {this->removeCacheBlock(tag); wait(this->cacheRemoveLatency);}
					else if (this->cache[tag].size() > numWays)
						THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Something wrong happened in the cache, too many blocks under the same tag");
					this->cache[tag].push_front(curBlock);
				}
			}

			// Finally, we can actually write the given data in the block (if there is a block)...
			if (hit || this->writePolicy == BACK || this->writePolicy == THROUGH_ALL) {
				// We calculate the displacement of the data in the cache block...
				cachePointerModifier = address - curBlock.base_address;
				if (cachePointerModifier < 0) cachePointerModifier = 0;
				// And the length of the data to be written
				partLen = remLen;
				if (partLen + cachePointerModifier > this->blockSize) partLen = this->blockSize - cachePointerModifier;
				// We perform the effective write...
				memcpy(curBlock.block + cachePointerModifier, dataPointer, partLen*sizeof(unsigned char));
				wait(this->cacheWriteLatency);
				// And we update the pointer to the required data for eventual subsequent writes
				remLen -= partLen;
				dataPointer += partLen;
			}
			// If the current cache block is not loaded (we use a pure Write-Through policy)...
			else {
				// We update the support variables to be ready for the eventual subsequent writes
				cachePointerModifier = address - address%blockSize;
				if (cachePointerModifier < 0) cachePointerModifier = 0;
				partLen = remLen;
				if (partLen + cachePointerModifier > this->blockSize) partLen = this->blockSize - cachePointerModifier;
				remLen -= partLen;
				dataPointer += partLen;
			}
		}
	}

	void b_transport(tlm_generic_payload& trans, sc_time& delay){
		tlm_command cmd = trans.get_command();
		sc_dt::uint64    adr = trans.get_address();
		unsigned char*   ptr = trans.get_data_ptr();
		unsigned int     len = trans.get_data_length();
		unsigned char*   byt = trans.get_byte_enable_ptr();
		unsigned int     wid = trans.get_streaming_width();

		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - Transaction " << trans.is_write() << " for Address " << adr << " with ";
		cerr << "data length " << len << " and streaming width " << wid << endl;
//		ofstream outFile("cache.txt", ios::app);
//		outFile << sc_time_stamp() << " " << trans.get_address() << " " << trans.get_data_length() << " " << trans.is_write() << endl;
		#endif			

		unsigned int words = len / sizeof(BUSWIDTH);
		if (len%sizeof(BUSWIDTH) != 0) words++;

		if(this->scratchpadEn && adr >= this->scratchStart && adr + len <= (this->scratchStart + this->scratchSize)) {
			//Using local scratchpad
			if(this->scratchMemory == NULL){
				THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Trying to access a NULL scratchpad");
			}
			if(cmd == TLM_READ_COMMAND)
				memcpy(ptr, &scratchMemory[adr - this->scratchStart], len);
			else if(cmd == TLM_WRITE_COMMAND)
				memcpy(&scratchMemory[adr - this->scratchStart], ptr, len);
			else THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Undefined TLM command");
			this->numScratchAcc++;
			wait(words*this->scratchLatency);
			trans.set_response_status(TLM_OK_RESPONSE);
			return;
		}
		else if (adr > cacheLimit){
			this->initSocket->b_transport(trans,delay);
		}
		else {
			if (cmd == TLM_READ_COMMAND) {
				this->readFromCache(adr, ptr, len);
				trans.set_response_status(TLM_OK_RESPONSE);
			}
			else if (cmd == TLM_WRITE_COMMAND) {
				this->writeToCache(adr, ptr, len);
				if (writePolicy == THROUGH || writePolicy == THROUGH_ALL) {
					this->initSocket->b_transport(trans,delay);
				}
				else trans.set_response_status(TLM_OK_RESPONSE);
			}
			else THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Undefined TLM command");
			this->numAccesses++;
		}

		trans.set_dmi_allowed(false);			// Disables DMI in order to insert the cache latency for each transaction
	}

	// TLM-2 DMI method
	bool get_direct_mem_ptr(tlm_generic_payload& trans, tlm_dmi& dmi_data){
		cerr << "DMI not supported for cache transactions!" << endl;
		return false;
	}

	// TLM-2 debug transaction method
	// It is unuseful to work with caches for DBG methods, since there is no time concept
	// This method simply forwards the request to the memory
	unsigned int transport_dbg(tlm_generic_payload& trans){
		tlm_command cmd = trans.get_command();
		sc_dt::uint64    adr = trans.get_address();
		unsigned char*   ptr = trans.get_data_ptr();
		unsigned int     len = trans.get_data_length();

		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - DBG Transaction " << trans.is_write() << " for Address " << adr << " with ";
		cerr << "data length " << len << endl;
		#endif			

		// Calculating the TAG associated to the required memory position
		sc_dt::uint64 tag = GET_TAG(adr);

		// Declaring some support variables...
		tlm_generic_payload message;
		deque<CacheBlock>::iterator tagIter;
		bool hit;
		CacheBlock curBlock;
		sc_dt::uint64 curBaseAddress;
		int cachePointerModifier;
		unsigned int partLen;
		unsigned int remLen = len;
		unsigned char *dataPointer = ptr;

		if(this->scratchpadEn && adr >= this->scratchStart && adr + len <= (this->scratchStart + this->scratchSize)) {
			//Using local scratchpad
			if(this->scratchMemory == NULL){
				THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Trying to access a NULL scratchpad");
			}
			if(cmd == TLM_READ_COMMAND)
				memcpy(ptr, &scratchMemory[adr - this->scratchStart], len);
			else if(cmd == TLM_WRITE_COMMAND)
				memcpy(&scratchMemory[adr - this->scratchStart], ptr, len);
			else THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Undefined TLM command");
			trans.set_response_status(TLM_OK_RESPONSE);
			return len;
		}
		else if (adr > cacheLimit){
			return this->initSocket->transport_dbg(trans);
		}

		// else ...
		// For each block containing part of the requested data...
		for (curBaseAddress = adr - (adr % this->blockSize); curBaseAddress < adr+len; curBaseAddress += this->blockSize) {
			hit = false;
			// For each block cached...
			// N.B. If no blocks are currently cached, cache[tag] is an ampty queue!
			for (tagIter = this->cache[tag].begin(); tagIter != this->cache[tag].end(); tagIter++) {
				// If the required block is cached...
				if (tagIter->base_address == curBaseAddress) {
					// We have a HIT!
					#ifdef DEBUGMODE
					cerr << sc_time_stamp() << ": " << name() << " - DBG Hit! @" << curBaseAddress << endl;
					#endif			
					hit = true;
					// We save the current block...
					curBlock = *(tagIter);
					// And we upgrade the queue order if we use an LRU policy
					if (this->removePolicy==LRU) {
						this->cache[tag].erase(tagIter);
						this->cache[tag].push_front(curBlock);
					}
					break;
				}
			}
			if (!hit) {
				// We have a MISS...
				#ifdef DEBUGMODE
				cerr << sc_time_stamp() << ": " << name() << " - DBG Miss! @ " << curBaseAddress << endl;
				#endif			

				// We load the block from memory...
				curBlock = CacheBlock();
				curBlock.block = (unsigned char*) malloc (blockSize*sizeof(unsigned char));
				curBlock.base_address = curBaseAddress;

				message.set_data_length(blockSize);
				message.set_data_ptr(curBlock.block);
				message.set_read();
				message.set_address(curBlock.base_address);
				message.set_response_status(TLM_INCOMPLETE_RESPONSE);
				this->initSocket->transport_dbg(message);
				if (message.get_response_status() != TLM_OK_RESPONSE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error while reading from main memory");
			}

			// Finally, we can actually read the required data from the block...

			// We calculate the displacement of the data in the cache block...
			cachePointerModifier = adr - curBlock.base_address;
			if (cachePointerModifier < 0) cachePointerModifier = 0;
			// And the length of the data to be read
			partLen = remLen;
			if (partLen + cachePointerModifier > this->blockSize) partLen = this->blockSize - cachePointerModifier;
			// We perform the effective read...
			if(cmd == TLM_READ_COMMAND)
				memcpy(dataPointer, curBlock.block + cachePointerModifier, partLen*sizeof(unsigned char));
			else if(cmd == TLM_WRITE_COMMAND) {
				memcpy(curBlock.block + cachePointerModifier, dataPointer, partLen*sizeof(unsigned char));

				message.set_data_length(blockSize);
				message.set_data_ptr(curBlock.block);
				message.set_write();
				message.set_address(curBlock.base_address);
				message.set_response_status(TLM_INCOMPLETE_RESPONSE);
				this->initSocket->transport_dbg(message);
				if (message.get_response_status() != TLM_OK_RESPONSE) THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Error while reading from main memory");
			}
			else THROW_EXCEPTION(__PRETTY_FUNCTION__ << ": Undefined TLM command");
			// And we update the pointer to the required data for eventual subsequent reads
			remLen -= partLen;
			dataPointer += partLen;
		}
		trans.set_response_status(TLM_OK_RESPONSE);

		#ifdef DEBUGMODE
		cerr << sc_time_stamp() << ": " << name() << " - DBG Transaction " << trans.is_write() << " for Address " << adr << " completed with data ";
		if (trans.get_data_length() == 4) cerr << (unsigned int) (*trans.get_data_ptr()) << endl;
		else cerr << "ND" << endl;
		#endif			

		return len;
	}
};

#endif

#include <stdio.h>
/* Version 30 October 2001 */

#ifndef RANGE_HEADER
#define RANGE_HEADER

#include <limits.h>

/* Static or adaptive model? */
#define  STATIC    0U
#define  ADAPT     1U

/* Error codes */
#define RC_OK        0U
#define RC_ERROR     1U
#define RC_IO_ERROR  2U

#if UINT_MAX >= 0xffffffffU
	typedef   signed int  Sint;
	typedef unsigned int  Uint;
#else
	typedef   signed long Sint;
	typedef unsigned long Uint;
#endif

typedef unsigned char uc;

#define  TOP       ((Uint)1 << 24)
#define  BOT       ((Uint)1 << 16)

typedef struct {
	Uint  low, range, passed, error;
	FILE *fp;
	uc   *ptr, *eptr;
} rc_encoder;

typedef struct {
	Uint  low, code, range, passed, error;
	FILE *fp;
	uc   *ptr, *eptr;
} rc_decoder;

typedef struct {
	Uint *freq, totFreq, incr, maxFreq, adapt, halfFreq, firstFreq, lastFreq;
	Uint lastSym, lastCumFreq;
	Uint nsym, nsym2, nsym3, nsym4, nsym23;
} rc_model;

Uint ModelInit    (rc_model *, Uint, Uint *, Uint *, Uint, Uint, uc);
void StartEncode  (rc_encoder *, FILE *, uc *, Uint);
void FinishEncode (rc_encoder *);
void StartDecode  (rc_decoder *, FILE *, uc *, Uint);
Sint EncodeSymbol (rc_encoder *, rc_model *, Uint);
Sint DecodeSymbol (rc_decoder *, rc_model *);

#endif

/* Carryless rangecoder (c) 1999 by Dmitry Subbotin      */
/* Modified into C and extended 2001 by Mikael Lundqvist */
/* Version 30 October 2001 */

#include <stdio.h>
#include <assert.h>

/* The two IO functions */
static
void OutByte (rc_encoder *rc, uc c) {
	if (rc->ptr) {
		if (rc->ptr < rc->eptr) {
			rc->passed++;
			*rc->ptr++ = c;
		}
		else
			rc->error = RC_IO_ERROR;
	}
	else if (rc->fp) {
		rc->passed++;
		if (putc(c, rc->fp) == EOF)
			rc->error = RC_IO_ERROR;
	}
	else
		rc->error = RC_IO_ERROR;
}

static
uc InByte (rc_decoder *rd) {
	int sym;
	
	if (rd->ptr) {
		if (rd->ptr < rd->eptr) {
			rd->passed++;
			return *rd->ptr++;
		}
		else
			rd->error = RC_IO_ERROR;
	}
	else if (rd->fp) {
		rd->passed++;
		sym = getc(rd->fp);
		if (sym == EOF)
			rd->error = RC_IO_ERROR;
		return sym;
	}
	else
		rd->error = RC_IO_ERROR;
	
	return 0;
}

/* Rangecoder start and end functions */
void StartEncode (rc_encoder *rc, FILE *F, uc *ptr, Uint length) {
	rc->fp = F;
	rc->passed = rc->low = 0;
	rc->range = (Uint) -1;
	rc->ptr = ptr;
	rc->eptr = ptr + length;
	rc->error = RC_OK;
}

void FinishEncode (rc_encoder *rc) {
	OutByte(rc, rc->low>>24), rc->low<<=8;
	OutByte(rc, rc->low>>24), rc->low<<=8;
	OutByte(rc, rc->low>>24), rc->low<<=8;
	OutByte(rc, rc->low>>24), rc->low<<=8;
}

void StartDecode (rc_decoder *rd, FILE *F, uc *ptr, Uint length) {
	rd->passed = rd->low = rd->code = 0;
	rd->range = (Uint) -1;
	rd->fp = F;
	rd->ptr = ptr;
	rd->eptr = ptr + length;
	rd->error = RC_OK;
	rd->code = rd->code<<8 | InByte(rd);
	rd->code = rd->code<<8 | InByte(rd);
	rd->code = rd->code<<8 | InByte(rd);
	rd->code = rd->code<<8 | InByte(rd);
}

/* Rangecoder frequency encoding and decoding */
static
void Encode (rc_encoder *rc, rc_model *rcm, Uint cumFreq, Uint sym) {
	/* assert(cumFreq+rcm->freq[sym] <= rcm->totFreq &&
			rcm->freq[sym] && rcm->totFreq <= rcm->maxFreq); */
	rc->low  += cumFreq * (rc->range /= rcm->totFreq);
	rc->range *= rcm->freq[sym];
	while ( (rc->low ^ (rc->low+rc->range)) < TOP || (rc->range < BOT &&
			((rc->range = -rc->low & (BOT-1)), 1)) )
	{	OutByte(rc, rc->low>>24);
		rc->range <<= 8;
		rc->low <<= 8;
	}
}

static
void Decode (rc_decoder *rd, rc_model *rdm, Uint cumFreq, Uint sym) {
	/* assert(cumFreq + rdm->freq[sym] <= rdm->totFreq &&
			rdm->freq[sym] && rdm->totFreq <= rdm->maxFreq); */
	rd->low  += cumFreq * rd->range;
	rd->range *= rdm->freq[sym];
	while ( (rd->low ^ (rd->low+rd->range)) < TOP || (rd->range < BOT &&
			((rd->range = -rd->low & (BOT-1)),1)) )
	{	rd->code = rd->code<<8 | InByte(rd);
		rd->range <<= 8;
		rd->low <<= 8;
	}
}

/* Symbol encode and decode */
Sint EncodeSymbol(rc_encoder *rc, rc_model *rcm, Uint sym)
{
	Uint i, cumFreq;
	
	if (sym >= rcm->nsym) {
		rc->error = RC_ERROR;
		return -1;
	}
	
	/* Calculate cumFreq choosing the shortest path possible */
	if (sym >= rcm->lastSym)
		if (rcm->nsym - sym > sym - rcm->lastSym) {
			for (i = rcm->lastSym, cumFreq = rcm->lastCumFreq; i < sym; cumFreq += rcm->freq[i++]) ;
			rcm->lastCumFreq = cumFreq;
			rcm->lastSym = sym;
		}
		else
			for (i = sym, cumFreq = rcm->totFreq; i < rcm->nsym; cumFreq -= rcm->freq[i++]) ;
	else
		if (rcm->lastSym >= sym + sym) {
			for (cumFreq = i = 0; i < sym; cumFreq += rcm->freq[i++]) ;
			if (rcm->adapt) rcm->lastCumFreq += rcm->incr;
		}
		else {
			for (i = sym, cumFreq = rcm->lastCumFreq; i < rcm->lastSym; cumFreq -= rcm->freq[i++]) ;
			rcm->lastCumFreq = cumFreq;
			rcm->lastSym = sym;
		}
	
	Encode(rc, rcm, cumFreq, sym);
	
	/* Update statistics */
	if (rcm->adapt) {
		rcm->freq[sym] += rcm->incr;
		rcm->totFreq += rcm->incr;
		if (rcm->totFreq > rcm->maxFreq) {
			for (i = rcm->totFreq = 0; i < rcm->nsym; i++)
				rcm->totFreq += (rcm->freq[i] -= (rcm->freq[i] >> 1));
			rcm->lastCumFreq = 0;
			rcm->lastSym = 0;
		}
	}
	
	return 0;
}

Sint DecodeSymbol(rc_decoder *rd, rc_model *rdm)
{
	Uint i, sym, cumFreq, rfreq;
	
	rfreq = (rd->code - rd->low) / (rd->range /= rdm->totFreq); /* GetFreq */
	if (rfreq >= rdm->totFreq) {
		rd->error = RC_ERROR;
		return -1;
	}
	
	/* Find symbol choosing the shortest possible path */
	if (rdm->lastCumFreq > rfreq)
		if (rfreq > rdm->halfFreq || (rdm->lastSym <= rdm->nsym23 && rfreq >= rdm->firstFreq)) {
			for (sym = rdm->lastSym-1, cumFreq = rdm->lastCumFreq-rdm->freq[sym];;) {
				if (cumFreq <= rfreq) break;
				cumFreq -= rdm->freq[--sym];
			}
			rdm->lastCumFreq = cumFreq;
			rdm->lastSym = sym;
		}
		else { /* if rfreq < halfFreq && (lastSym > nsym23 || rfreq < firstFreq) */
			for (sym = 0, cumFreq = rdm->freq[0];;) {
				if (cumFreq > rfreq) break;
				cumFreq += rdm->freq[++sym];
			}
			cumFreq -= rdm->freq[sym];
			if (rdm->adapt) rdm->lastCumFreq += rdm->incr;
		}
	else /* lastCumFreq <= rfreq */
		if (rfreq < rdm->halfFreq || (rdm->lastSym >= rdm->nsym3 && rfreq <= rdm->lastFreq)) {
			for (sym = rdm->lastSym, cumFreq = rdm->lastCumFreq + rdm->freq[sym];;) {
				if (cumFreq > rfreq) break;
				cumFreq += rdm->freq[++sym];
			}
			cumFreq -= rdm->freq[sym];
			rdm->lastCumFreq = cumFreq;
			rdm->lastSym = sym;
		}
		else /* if rfreq > halfFreq && (rdm->lastSym < rdm->nsym3 || rfreq > lastFreq) */
			for (sym = rdm->nsym-1, cumFreq = rdm->totFreq-rdm->freq[sym];;) {
				if (cumFreq <= rfreq) break;
				cumFreq -= rdm->freq[--sym];
			}
	
	Decode(rd, rdm, cumFreq, sym);
	
	/* Update statistics */
	if (rdm->adapt) {
		rdm->freq[sym] += rdm->incr;
		if (sym < rdm->nsym23) {
			rdm->lastFreq += rdm->incr;
			if (sym < rdm->nsym2) {
				rdm->halfFreq += rdm->incr;
				if (sym < rdm->nsym4)
					rdm->firstFreq += rdm->incr;
			}
		}
		rdm->totFreq += rdm->incr;
		if (rdm->totFreq > rdm->maxFreq) {
			for (i = rdm->totFreq = 0; i < rdm->nsym; i++)
				rdm->totFreq += (rdm->freq[i] -= (rdm->freq[i] >> 1));
			rdm->halfFreq /= 2;
			rdm->firstFreq /= 2;
			rdm->lastFreq /= 2;
			rdm->lastCumFreq = 0;
			rdm->lastSym = 0;
		}
	}
	
	return sym;
}

/* Initializing the model */
Uint ModelInit(rc_model *rcm, Uint nsym, Uint *ftbl, Uint *ifreq,
				Uint incr, Uint maxFreq, uc adapt) {
	Uint i;
	
	rcm->nsym = nsym;  /* Nr of symbols */
	/* nsym2 - nsym23 are just constants designed to speed things up */
	rcm->nsym2 = nsym/2;
	rcm->nsym3 = nsym/3;
	rcm->nsym4 = nsym*5/22;  /* For firstFreq */
	rcm->nsym23 = nsym*2/3;  /* For lastFreq */
	
	rcm->freq = ftbl;
	rcm->totFreq = 0;
	if (ifreq != NULL)
		for (i = 0; i < nsym; i++)
			rcm->totFreq += (ftbl[i] = ifreq[i]);
	else
		for (i = 0; i < nsym; i++)
			rcm->totFreq += (ftbl[i] = 1);
	
	/* Sums to help DecodeSymbol make better decisions */
	for (rcm->firstFreq = i = 0; i < rcm->nsym4; i++)
		rcm->firstFreq += rcm->freq[i];
	for (rcm->halfFreq = rcm->firstFreq, i = rcm->nsym4; i < rcm->nsym2; i++)
		rcm->halfFreq += rcm->freq[i];
	for (rcm->lastFreq = rcm->halfFreq, i = rcm->nsym2; i < rcm->nsym23; i++)
		rcm->lastFreq += rcm->freq[i];
	
	rcm->incr = incr;
	rcm->maxFreq = maxFreq;
	rcm->adapt = adapt;
	rcm->lastCumFreq = 0;
	rcm->lastSym = 0;
	
	/* Total frequency MUST be kept below 1 << 16 (= 65536) */
	if (rcm->maxFreq > BOT || rcm->totFreq > BOT)
		return RC_ERROR;
		
	return RC_OK;
}


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BSIZE 2000000
#define CSIZE BSIZE + BSIZE/100
#define SYMS 256

Uint ftbl[SYMS];  /* The frequency table */

/* A simple demonstration of memory to memory compression */
int main () {
	Sint sym;
	Uint i, n;
	uc *op, *s;
	rc_encoder rc;
	rc_decoder rd;
	rc_model rcm;
	
	op = (uc *)malloc(CSIZE);
	s = (uc *)malloc(BSIZE+1);
	srand(clock());
	if (op && s) {
		/* King's worst case expanded 
		for (i = 0; i < BSIZE-3; i+=3) {
			s[i] = rand() % 1 + 0x53;
			s[i+1] = rand() % 1 + 0xa8;
			s[i+2] = rand() % 1 + 0xfe;
		}
		*/
		for (i = 0; i < BSIZE-2; i+=2) {
			s[i] = 0x55;
			s[i+1] = 0xaa;
		}
		
		n = ModelInit (&rcm, SYMS, ftbl, NULL, 4, (Uint)1<<16, ADAPT);
		if (n != RC_OK) {
			fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
			free(op);
			free(s);
			exit(1);
		}
		fprintf(stderr, "Encoding...\n");
		fflush(stdout);
		StartEncode (&rc, NULL, op, CSIZE);
		for (i = 0; i < BSIZE; i++) {
			EncodeSymbol (&rc, &rcm, s[i]);
			switch (rc.error) {
				case RC_OK:
					break;
				case RC_ERROR:
					fprintf(stderr, "Encode error: Symbol outside range\n");
					free(op);
					free(s);
					exit(1);
				case RC_IO_ERROR:
					fprintf(stderr, "Encode error: Could not output compressed data\n");
					free(op);
					free(s);
					exit(1);
			}
		}
		FinishEncode (&rc);
		fprintf(stderr, "Data length: %ld, passed: %lu\n", (long)BSIZE, (unsigned long)rc.passed);
		
		n = ModelInit (&rcm, SYMS, ftbl, NULL, 4, (Uint)1<<16, ADAPT);
		if (n != RC_OK) {
			fprintf(stderr, "Error: maxFreq or totFreq > 65536\n");
			free(op);
			free(s);
			exit(1);
		}
		fprintf(stderr, "Decoding...\n");
		fflush(stdout);
		StartDecode (&rd, NULL, op, CSIZE);
		n = 0;
		for (i = 0; i < BSIZE; i++) {
			sym = DecodeSymbol (&rd, &rcm);
			switch (rd.error) {
				case RC_OK:
					break;
				case RC_ERROR:
					fprintf(stderr, "Decode error: Symbol outside range\n");
					free(op);
					free(s);
					exit(1);
				case RC_IO_ERROR:
					fprintf(stderr, "Decode error: Could not input compressed data\n");
					free(op);
					free(s);
					exit(1);
			}
			if (s[n++] != sym) break;
		}
		
		fprintf(stderr, "Done!\n");
		fflush(stdout);
		if (i == BSIZE) {
			fprintf(stderr, "Data length: %ld, passed: %lu\n", (long)BSIZE, (unsigned long)rc.passed);
			fprintf(stderr, "Strings are the same!\n");
		}
		else
			fprintf(stderr, "Strings differ at %d...\n", i);
	}
	free(op);
	free(s);
	
	return 0;
}

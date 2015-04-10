#include <stdio.h>
#include <stdlib.h>
#include <quantum.h>
#include "bb84.h"

/*****
// States:
// QREADY - I have no data pending output to classic, send me qbits
// QSET - You have told me to read qbits
// QWAIT - I have pending data output to classic
*****/

#define STATE_QREADY 	0
#define STATE_QSET 	1
#define STATE_QWAIT 	2

static const char QUANTUM_DAT_PIPE[] = "/tmp/qdatpipe";
static const char QUANTUM_SIG_PIPE[] = "/tmp/qsigpipe";
static const char CLASSIC_DAT_PIPE[] = "/tmp/cdatpipe";
static const char CLASSIC_SIG_PIPE[] = "/tmp/csigpipe";

void readBitPipe(FILE *pipe, char);

int main() {
	FILE *qdp, *qsp, *cdp, *csp;
	int qPipeState = 0, cPipeState = 0;
		

	qip = fopen ( QUANTUM_DAT_PIPE, "w+");
	qop = fopen ( QUANTUM_SIG_PIPE, "w+");
	cip = fopen ( CLASSIC_DAT_PIPE, "w+");
	cop = fopen ( CLASSIC_SIG_PIPE, "w+");

	while ( 1 == 1 ) {
		switch ( sPipeState ) {
			case STATE_QREADY:
			case STATE_QWAIT:
				break;
			case STATE_QSET:
				//read the data pipe
				break;
			default:
				break;
		}

	}		

	return 0;
}

void readPipe(FILE *pipe, char *readData) {
	

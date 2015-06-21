#include "msg/msg.h"
#include "reputation_strategy.h"
#include "simulator.h"


void updateReputation_Sonnek (struct p_worker * worker) {
	worker->reputation = (int)((((double)worker->totC + 1.0) / ((double)worker->totR + 2.0)) * 100.0);
}


void updateReputation_Symmetrical (struct p_worker * worker, char good_or_bad_answer) {
	if (good_or_bad_answer == 1) {
		printf("%s have is reputation increased to %d\n", worker->mailbox, worker->reputation + (int)((double)worker->reputation * reputation_x)); 
		worker->reputation = worker->reputation + (int)((double)worker->reputation * reputation_x);
		if (worker->reputation > MAX_REPUTATION) {
			worker->reputation = MAX_REPUTATION;
		}
	}
	else {
		printf("%s have is reputation decreased to %d\n", worker->mailbox, worker->reputation - (int)((double)worker->reputation * reputation_x)); 
		worker->reputation = worker->reputation - (int)((double)worker->reputation * reputation_x);
		if (worker->reputation < MIN_REPUTATION) {
			worker->reputation = MIN_REPUTATION;
		}
	}
}


void updateReputation_Asymmetrical (struct p_worker * worker, char good_or_bad_answer) {
	if (good_or_bad_answer == 1) {
		worker->reputation = worker->reputation + (int)((double)worker->reputation * reputation_x);
		if (worker->reputation > MAX_REPUTATION) {
			worker->reputation = MAX_REPUTATION;
		}
	}
	else {
		worker->reputation = worker->reputation - (int)((double)worker->reputation * reputation_y);
		if (worker->reputation < MIN_REPUTATION) {
			worker->reputation = MIN_REPUTATION;
		}
	}
}


void updateReputation_BOINC (struct p_worker * worker) {
	if (worker->totR == 0) {
		worker->reputation = 100;
	}
	else {
		worker->reputation = (int) ((1.0 - (double) ((((double)worker->totR - (double)worker->totC) / (double)(worker->totR)))) * 100);
	}
}

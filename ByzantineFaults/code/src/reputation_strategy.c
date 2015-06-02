#include "msg/msg.h"
#include "reputation_strategy.h"


void updateReputation_Sonnek (struct p_worker * worker) {
	worker->reputation = (int)((((double)worker->totC + 1.0) / ((double)worker->totR + 2.0)) * 100.0);
	printf("new value of the reputation of worker %s: %d\n", worker->mailbox, (int)((((double)worker->totC + 1.0) / ((double)worker->totR + 2.0)) * 100.0));
}


void updateReputation_Symmetrical (struct p_worker * worker, double x, char good_or_bad_answer) {
	if (good_or_bad_answer == 1) {
		worker->reputation = worker->reputation + (int)((double)worker->reputation * x);
		if (worker->reputation > MAX_REPUTATION) {
			worker->reputation = MAX_REPUTATION;
		}
	}
	else {
		worker->reputation = worker->reputation - (int)((double)worker->reputation * x);
		if (worker->reputation < MIN_REPUTATION) {
			worker->reputation = MIN_REPUTATION;
		}
	}
}


void updateReputation_Asymmetrical (struct p_worker * worker, double x, double y, char good_or_bad_answer) {
	if (good_or_bad_answer == 1) {
		worker->reputation = worker->reputation + (int)((double)worker->reputation * x);
		if (worker->reputation > MAX_REPUTATION) {
			worker->reputation = MAX_REPUTATION;
		}
	}
	else {
		worker->reputation = worker->reputation - (int)((double)worker->reputation * y);
		if (worker->reputation < MIN_REPUTATION) {
			worker->reputation = MIN_REPUTATION;
		}
	}
}


void updateReputation_BOINC (struct p_worker * worker) {
	worker->reputation = (int) ((1.0 - (double) ((((double)worker->totR - (double)worker->totC) / (double)(worker->totR)))) * 100);
}

#include "msg/msg.h"
#include "reputation_strategy.h"
#include "simulator.h"


void updateReputation_Sonnek (struct p_worker * worker) {
	worker->reputation = (int)((((double)worker->totC + 1.0) / ((double)worker->totR + 2.0)) * 100.0);

	msg_task_t task_complexity = MSG_task_create("task_complexity", 5.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void updateReputation_Symmetrical (struct p_worker * worker, char good_or_bad_answer) {
	double complexity = 0.0;

	if (good_or_bad_answer == 1) {
		//printf("%s have is reputation increased to %d\n", worker->mailbox, worker->reputation + (int)((double)worker->reputation * reputation_x)); 
		worker->reputation = worker->reputation + (int)((double)worker->reputation * reputation_x);
		if (worker->reputation > MAX_REPUTATION) {
			worker->reputation = MAX_REPUTATION;
			complexity++;
		}
	}
	else {
		//printf("%s have is reputation decreased to %d\n", worker->mailbox, worker->reputation - (int)((double)worker->reputation * reputation_x)); 
		worker->reputation = worker->reputation - (int)((double)worker->reputation * reputation_x);
		if (worker->reputation < MIN_REPUTATION) {
			worker->reputation = MIN_REPUTATION;
			complexity++;
		}
	}
	complexity += 5.0;

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void updateReputation_Asymmetrical (struct p_worker * worker, char good_or_bad_answer) {
	double complexity = 0.0;

	if (good_or_bad_answer == 1) {
		worker->reputation = worker->reputation + (int)((double)worker->reputation * reputation_x);
		if (worker->reputation > MAX_REPUTATION) {
			worker->reputation = MAX_REPUTATION;
			complexity++;
		}
	}
	else {
		worker->reputation = worker->reputation - (int)((double)worker->reputation * reputation_y);
		if (worker->reputation < MIN_REPUTATION) {
			worker->reputation = MIN_REPUTATION;
			complexity++;
		}
	}
	complexity += 5.0;

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void updateReputation_BOINC (struct p_worker * worker) {
	double complexity = 0.0;

	if (worker->totR == 0) {
		worker->reputation = 100;
		complexity = 2.0;
	}
	else {
		worker->reputation = (int) ((1.0 - (double) ((((double)worker->totR - (double)worker->totC) / (double)(worker->totR)))) * 100);
		//printf("dans update_reputation_BOINC: value of %s %d\n", worker->mailbox, worker->reputation); 
		complexity = 6.0;
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}

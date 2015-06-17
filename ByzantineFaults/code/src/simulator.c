#include "msg/msg.h"
#include "primary.h"
#include "worker.h"
#include "client.h"
#include "simulator.h"

int main (int argc, char * argv[]) {
	int index = 1;

	if (argc < 6) {
		printf("you are using a simulator simulating a probabilistic centralised replication algorithm\n");
		printf("usage: ./my-boinc SIMULATOR REPUTATION_STRATEGY FORMATION_GROUP_STRATEGY ADDITIONAL_REPLICATION_STRATEGY\n");
		printf("SIMULATOR can take two different values: SONNEK or ARANTES\n");
		printf("REPUTATION_STRATEGY can take 4 different values: SYMMETRICAL, ASYMMETRICAL, BOINC, SONNEK_REPUTATION\n");
		printf("if you use SYMMETRICAL you need to precise just behind the value x of which the reputation will be increase or decrease\n");
		printf("if you use ASYMMETRICAL you need to precise juste behind the value x and y that respectively will serve to increase and decrease the reaputation of the workers\n");
		printf("FORMATION_GROUP_STRATEGY can take 4 different values: FIXED_FIT, FIRST_FIT, TIGHT_FIT, RANDOM_FIT\n");
		printf("if you use FIXED-FIT you need to precise just behind the value of workers you want in each groups\n");
		printf("if you use FIRST-FIT, TIGHT-FIT or RANDOM-FIT you need to precise the value you want to achieve for each group, the minimum number of workers you want in each group and the maximum number of workers you want in each group\n");
		printf("ADDITIONAL_REPLICATION_STRATEGY can take 3 different values: ARANTES_REPLICATION, ITERATIVE_REDUNDANCY, PROGRESSIVE_REDUNDANCY\n");
		printf("if you use ITERATIVE_REDUNDANCY you need to precise the number of answers you want between the supposed good and the supposed bad answers\n");
		exit(1);
	}

	if (!strcmp(argv[index], "SONNEK")) {
		simulator = SONNEK;
	}
	else if (!strcmp(argv[index], "ARANTES")) {
		simulator = ARANTES;
	}
	else {
		printf("the parameter %d must have the value SONNEK or ARANTES\n", index);
		exit(1);
	}
	index++;

	if (!strcmp(argv[index], "SYMMETRICAL")) {
		reputation_strategy = SYMMETRICAL;
		index++;
		if ((reputation_x = atoi(argv[index])) == 0) {
			printf("just after parameter SYMMETRICAL you need to precise the value x that will serve to increase and decrease the reputation\n");
			exit(1);
		}
		index++;
	}
	else if (!strcmp(argv[index], "ASYMMETRICAL")) {
		reputation_strategy = ASYMMETRICAL;
		index++;
		if ((reputation_x = atoi(argv[index])) == 0) {
			printf("just after parameter ASYMMETRICAL you need to precise the value x that will serve to increase reputations\n");
			exit(1);
		}
		index++;
		if ((reputation_y = atoi(argv[index])) == 0) {
			printf("when ASYMMETRICAL is choose the parameter %d must be the value y that will serve to decrease reputations\n", index);
			exit(1);
		}
		index++;
	}
	else if (!strcmp(argv[index], "BOINC")) {
		reputation_strategy = BOINC;
		index++;
	}
	else if (!strcmp(argv[index], "SONNEK_REPUTATION")) {
		reputation_strategy = SONNEK_REPUTATION;
		index++;
	}
	else {
		printf("the parameter %d must have the value SYMMETRICAL, ASYMMETRICAL, BOINC or SONNEK_REPUTATION\n", index);
		exit(1);
	}

	if (!strcmp(argv[index], "FIXED_FIT")) {
		group_formation_strategy = FIXED_FIT;
		index++;
		if ((group_formation_fixed_number = atoi(argv[index])) == 0) {
			printf("just after parameter FIXED_FIT you need to precise the number of workers you want in each groups\n");
			exit(1);
		}
		index++;
	}
	else if (!strcmp(argv[index], "FIRST_FIT")) {
		group_formation_strategy = FIRST_FIT;
	}
	else if (!strcmp(argv[index], "TIGHT_FIT")) {
		group_formation_strategy = TIGHT_FIT;
	}
	else if (!strcmp(argv[index], "RANDOM_FIT")) {
		group_formation_strategy = RANDOM_FIT;
	}
	else {
		printf("the parameter %d must have the value FIXED_FIT, FIRST_FIT, TIGHT_FIT or RANDOM_FIT\n", index); 
	}

	if ((group_formation_strategy == FIRST_FIT) || (group_formation_strategy == TIGHT_FIT) || (group_formation_strategy == RANDOM_FIT)) {
		index++;
		if ((group_formation_target_value = atoi(argv[index])) == 0) {
			printf("just after parameter FIRST_FIT, TIGHT_FIT or RANDOM_FIT you need to precise the target value you want each groups achieve\n");
			exit(1);
		}
		index++;
		if ((group_formation_min_number = atoi(argv[index])) == 0) {
			printf("the parameter %d must be the minimum number of workers you want in each group\n", index);
			exit(1);
		}
		index++;
		if ((group_formation_max_number = atoi(argv[index])) == 0) {
			printf("the parameter %d must be the maximum number of workers you want in each group\n", index);
			exit(1);
		}
		index++;
	}

	if (!strcmp(argv[index], "ARANTES_REPLICATION")) {
		additional_replication_strategy = ARANTES_REPLICATION;
	}
	else if (!strcmp(argv[index], "ITERATIVE_REDUNDANCY")) {
		additional_replication_strategy = ITERATIVE_REDUNDANCY;	
		index++;
		if ((additional_replication_value_difference = atoi(argv[index])) == 0) {
			printf("the parameter %d must be the difference between the supposed good and the supposed bad answers tou want to validate an answer\n", index);
			exit(1);
		}
	}
	else if (!strcmp(argv[index], "PROGRESSIVE_REDUNDANCY")) {
		additional_replication_strategy = PROGRESSIVE_REDUNDANCY;
	}
	else {
		printf("the parameter %d must have the value ARANTES_REPLICATION, ITERATIVE_REDUNDANCY or PROGRESSIVE_REDUNDANCY\n", index);
	}

	msg_error_t res = MSG_OK;

	MSG_init(&argc, argv);

	MSG_function_register("client", client);
	MSG_function_register("primary", primary);
	MSG_function_register("worker", worker);

	MSG_create_environment("../platforms/plat_finalize.xml");
	MSG_launch_application("../platforms/dep_finalize.xml");

	res = MSG_main();

	if (res == MSG_OK) {
		return 0;
	}
	else {
		return 1;
	}
}
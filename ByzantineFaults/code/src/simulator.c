#include "msg/msg.h"
#include "primary.h"
#include "worker.h"
#include "client.h"
#include "simulator.h"
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void print_workers_presence() {
	int i;
	
	for (i = 0; i < nb_workers; i++) {
		printf("value in presence[%d]\n", i);
		struct present_or_failed p_o_f;
		unsigned cpt;

		xbt_dynar_foreach(presence[i], cpt, p_o_f) {
			printf("%f, %d\n", p_o_f.time, p_o_f.type);
		}		
	}
}


void fill_workers_presence_array(char * file_database) {
	int fd;
	int nb_lines_read = 0;
	int i;
	int error;
	
	if ((fd = open(file_database, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
		printf("error open\n");
		exit(1);
	}

	read(fd, &time_start, sizeof(double));
	printf("value of time_start %f\n", time_start);

	presence = (xbt_dynar_t *) malloc(sizeof(xbt_dynar_t) * nb_workers);

	for (i = 0; i < nb_workers; i++) {
		presence[i] = xbt_dynar_new(sizeof(struct present_or_failed), NULL);
	}

	while(nb_lines_read != nb_workers) {
		double event_time;
		unsigned char type;

		if ((error = read(fd, &event_time, sizeof(double))) == 0) {
			// end of the file
			break;
		}
		if (event_time == -1.0) {
			nb_lines_read++;
		}
		else {
			if ((error = read(fd, &type, sizeof(unsigned char))) == 0) {
				printf("error: read error %d\n", error);
				exit(1);
			}
			struct present_or_failed * toAdd = (struct present_or_failed *) malloc(sizeof(struct present_or_failed)); 
			toAdd->time = event_time;
			toAdd->type = type;
			
			xbt_dynar_push(presence[nb_lines_read], toAdd);
		}
	} 

	print_workers_presence();
}


int main (int argc, char * argv[]) {
	int index = 1;
	char dep_file[FILE_NAME_SIZE];
	char plat_file[FILE_NAME_SIZE];

	if (argc < 8) {
		printf("you are using a simulator simulating a probabilistic centralised replication algorithm\n");
		printf("usage: ./my-boinc file_database number_workers dep_file plat_file SIMULATOR REPUTATION_STRATEGY FORMATION_GROUP_STRATEGY ADDITIONAL_REPLICATION_STRATEGY\n");
		printf("file_database corresponds to the traces you want to use to simulate the behavior of your nodes\n");
		printf("the number_workers corresponds to the number of worker you want in the system\n");
		printf("the dep_file and the plat_file correspond respectively to the xml file describing the deployment and the platform\n");
		printf("SIMULATOR can take two different values: SONNEK or ARANTES\n");
		printf("REPUTATION_STRATEGY can take 4 different values: SYMMETRICAL, ASYMMETRICAL, BOINC, SONNEK_REPUTATION\n");
		printf("if you use SYMMETRICAL you need to precise just behind the value x of which the reputation will be increase or decrease\n");
		printf("if you use ASYMMETRICAL you need to precise juste behind the value x and y that respectively will serve to increase and decrease the reaputation of the workers\n");
		printf("FORMATION_GROUP_STRATEGY can take 4 different values: FIXED_FIT, FIRST_FIT, TIGHT_FIT, RANDOM_FIT\n");
		printf("if you use FIXED-FIT you need to precise just behind the value of workers you want in each groups\n");
		printf("if you use FIRST-FIT, TIGHT-FIT or RANDOM-FIT you need to precise the value you want to achieve for each group, the minimum number of workers you want in each group and the maximum number of workers you want in each group\n");
		printf("you need to precise a replication strategy only if you are using the simulator ARANTES with a FIXED_FIT formation group strategy\n");
		printf("ADDITIONAL_REPLICATION_STRATEGY can take 3 different values: ITERATIVE_REDUNDANCY, PROGRESSIVE_REDUNDANCY\n");
		printf("if you use ITERATIVE_REDUNDANCY you need to precise the number of answers you want between the supposed good and the supposed bad answers\n");
		exit(1);
	}
	

	index++;
	if ((nb_workers = atoi(argv[index])) == 0) {
		printf("the parameter %d must be the number of workers you want in the systems\n", index);
	}
	index++;	
	strcpy(dep_file, argv[index]);
	index++;
	strcpy(plat_file, argv[index]);
	index++;

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
		if ((reputation_x = atof(argv[index])) == 0.0) {
			printf("just after parameter SYMMETRICAL you need to precise the value x that will serve to increase and decrease the reputation\n");
			exit(1);
		}
		index++;
	}
	else if (!strcmp(argv[index], "ASYMMETRICAL")) {
		reputation_strategy = ASYMMETRICAL;
		index++;
		if ((reputation_x = atof(argv[index])) == 0.0) {
			printf("just after parameter ASYMMETRICAL you need to precise the value x that will serve to increase reputations\n");
			exit(1);
		}
		index++;
		if ((reputation_y = atof(argv[index])) == 0.0) {
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
		group_formation_min_number = group_formation_fixed_number;
		group_formation_max_number = group_formation_fixed_number;
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

	
	if ((simulator == ARANTES) && (group_formation_strategy == FIXED_FIT)) { 
		if (!strcmp(argv[index], "ITERATIVE_REDUNDANCY")) {
			additional_replication_strategy = ITERATIVE_REDUNDANCY;	
			index++;
			if ((additional_replication_value_difference = atoi(argv[index])) == 0) {
				printf("the parameter %d must be the difference between the supposed good and the supposed bad answers you want to validate an answer\n", index);
				exit(1);
			}
		}
		else if (!strcmp(argv[index], "PROGRESSIVE_REDUNDANCY")) {
			additional_replication_strategy = PROGRESSIVE_REDUNDANCY;
			group_formation_min_number = floor((double)group_formation_fixed_number / 2.0) + 1;
		}
		else {
			printf("the parameter %d must have the value ARANTES_REPLICATION, ITERATIVE_REDUNDANCY or PROGRESSIVE_REDUNDANCY\n", index);
		}
	}

	fill_workers_presence_array(argv[1]);

	// running the simulation
	msg_error_t res = MSG_OK;

	MSG_init(&argc, argv);

	MSG_function_register("client", client);
	MSG_function_register("primary", primary);
	MSG_function_register("worker", worker);

	MSG_create_environment(plat_file);
	MSG_launch_application(dep_file);

	res = MSG_main();

	if (res == MSG_OK) {
		return 0;
	}
	else {
		return 1;
	}
}

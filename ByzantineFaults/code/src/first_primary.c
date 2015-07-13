#include "simulator.h"
#include "first_primary.h"
#include "msg/msg.h"
#include "client.h"
#include "primary.h"


void send_finalize_to_primaries () {
	unsigned int cpt;
	struct primary p;
	
	xbt_dynar_foreach(active_primaries, cpt, p) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, p.name);
	}

	unsigned int nb;
	struct primary primary;

	xbt_dynar_foreach(active_primaries, nb, primary) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, primary.name);
	}
}


void send_task_random(msg_task_t task) {
	int nb_rand = rand() % (xbt_dynar_length(active_primaries));
	struct primary * p = xbt_dynar_get_ptr(active_primaries, nb_rand);

	if (!strcmp(MSG_task_get_name(task), "join")) {
		MSG_task_send(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), MSG_task_get_data(task)), p->name);
	}
	else {
		MSG_task_send(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), TASK_MESSAGE_SIZE, MSG_task_get_data(task)), p->name);
	}
}


void send_task_reputations(msg_task_t task) {
	unsigned int cpt;
	struct primary p;
	struct clientDataTask * data = (struct clientDataTask *) malloc(sizeof(struct clientDataTask)); 
	struct primary * pSend;

	data = MSG_task_get_data(task);

	xbt_dynar_foreach(active_primaries, cpt, p) {
		if ((data->target_LOC >= p.min_reputation) && (data->target_LOC < p.max_reputation)) {
			pSend = xbt_dynar_get_ptr(active_primaries, cpt);
			break;
		}
	}
	if (!strcmp(MSG_task_get_name(task), "join")) {
		MSG_task_send(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), data), pSend->name);
	}
	else {
		MSG_task_send(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), TASK_MESSAGE_SIZE, data), pSend->name);
	}
}


int first_primary (int argc, char * argv[]) {
	unsigned long int id;
	char myMailbox[MAILBOX_SIZE];
	msg_task_t task;
	int nb_clients;
	int nb_finalize = 0;
	int i;

	if (argc != 3) {
		exit(1);
	}

	id = atoi(argv[1]);
	sprintf(myMailbox, "first-primary-%ld", id);
	
	srand(id);

	active_primaries = xbt_dynar_new(sizeof(struct primary), NULL);
	inactive_primaries = xbt_dynar_new(sizeof(struct primary), NULL);

	struct primary * p0 = (struct primary *) malloc(sizeof(struct primary));
	strcpy(p0->name, "primary-0"); 
	p0->max_reputation = 100;
	p0->min_reputation = 50;

	xbt_dynar_push(active_primaries, p0);

	for (i = 1; i < NB_MAX_ACTIVE_PRIMARIES; i++) {
		struct primary * p = (struct primary *) malloc(sizeof(struct primary));
		sprintf(p->name, "primary-%d", i); 
		p->max_reputation = 100;
		p->min_reputation = 50;

		xbt_dynar_push(inactive_primaries, p);
	}

	nb_clients = atoi(argv[2]);

	while (1) {
		// reception of a message
		MSG_task_receive(&(task), myMailbox);

		/* the primary can receive 4 types of messages: requests from client, 
			finalization from client, join from workers, or answer to tasks of workers */
		if (!strcmp(MSG_task_get_name(task), "finalize")) {
			printf("%s: I receive finalize\n", myMailbox);
			MSG_task_destroy(task);
			task = NULL;
			nb_finalize++;	
			if (nb_finalize == nb_clients) {			
				// if all clients have finish to send requests, the primary ask the workers to stop
				send_finalize_to_primaries(); 
				break;
			}
		}
		else if (!strncmp(MSG_task_get_name(task), "task", sizeof(char) * strlen("task"))) {
			if (distributed_strategies == RANDOM) {
				send_task_random(task);	
			}
			else {
				send_task_reputations(task);
			}
			MSG_task_destroy(task);
			task = NULL;
		}
		else if (!strcmp(MSG_task_get_name(task), "join")) {
			if (distributed_strategies == RANDOM) {
				send_task_random(task);
			}
			else {
				send_task_reputations(task);
			}
			MSG_task_destroy(task);
			task = NULL;
		}
		else {
			// messages incorrect
			printf("%s: I receive an incorrect message\n", myMailbox);
			MSG_task_destroy(task);
			task = NULL;
		}
	}

	return 0;
}

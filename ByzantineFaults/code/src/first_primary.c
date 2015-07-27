#include "simulator.h"
#include "first_primary.h"
#include "msg/msg.h"
#include "client.h"
#include "primary.h"


char able_division = 1;

char able_fusion = -1;


void send_finalize_to_primaries () {
	unsigned int cpt;
	struct primary p;
	
	xbt_dynar_foreach(active_primaries, cpt, p) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, p.name);
	}

	printf("first-primary %ld\n", xbt_dynar_length(inactive_primaries));

	unsigned int nb;
	struct primary primary;

	xbt_dynar_foreach(inactive_primaries, nb, primary) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, primary.name);
	}
}


void send_task_random(msg_task_t task) {
	int nb_rand = rand() % (xbt_dynar_length(active_primaries));
	struct primary * p = xbt_dynar_get_ptr(active_primaries, nb_rand);

	if (!strcmp(MSG_task_get_name(task), "join")) {
		printf("first-primary: I forward the join to primary-%d %s\n", nb_rand, MSG_task_get_name(task));
		MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), MSG_task_get_data(task)), p->name);
	}
	else {
		printf("first-primary: I forward the %s to primary-%d\n", MSG_task_get_name(task), nb_rand);
		MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), TASK_MESSAGE_SIZE, MSG_task_get_data(task)), p->name);
	}
}


void send_join_reputations(msg_task_t task) {
	unsigned int cpt;
	struct primary p;
	struct clientDataTask * data = (struct clientDataTask *) malloc(sizeof(struct clientDataTask)); 
	struct primary * pSend;

	data = MSG_task_get_data(task);

	xbt_dynar_foreach(active_primaries, cpt, p) {
		if ((50 >= p.min_reputation) && (50 < p.max_reputation)) {
			pSend = xbt_dynar_get_ptr(active_primaries, cpt);
			break;
		}
	}

	MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), data), pSend->name);
}


void send_task_reputations(msg_task_t task) {
	unsigned int cpt;
	struct primary p;
	struct clientDataTask * data = (struct clientDataTask *) malloc(sizeof(struct clientDataTask)); 
	struct primary * pSend;

	data = MSG_task_get_data(task);

	xbt_dynar_foreach(active_primaries, cpt, p) {
		if ((data->rangeReputationPrimaryToRequest >= ((double)p.min_reputation / 100.0)) && (data->rangeReputationPrimaryToRequest < ((double)p.max_reputation / 100.0))) {
			pSend = xbt_dynar_get_ptr(active_primaries, cpt);
			break;
		}
	}

	MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), TASK_MESSAGE_SIZE, data), pSend->name);
}


void treat_change(msg_task_t task) {
	xbt_dynar_t * workers_array = (xbt_dynar_t *) malloc(sizeof(xbt_dynar_t));
	*workers_array = xbt_dynar_new(sizeof(struct p_worker), NULL);


	workers_array = (xbt_dynar_t *)MSG_task_get_data(task);
	printf("size of the workers_array %ld\n", xbt_dynar_length(*workers_array));

	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach(*workers_array, cpt, p_w) {
		unsigned int nb;
		struct primary p;
		struct p_worker * workerToSend = (struct p_worker *)malloc(sizeof(struct p_worker));
		struct primary * toSend;
		
		printf("worker treated: name=%s, reputation=%d\n", p_w.mailbox, p_w.reputation);

		xbt_dynar_foreach(active_primaries, nb, p) {
			if ((p_w.reputation >= p.min_reputation) && (p_w.reputation < p.max_reputation)) {
				toSend = xbt_dynar_get_ptr(active_primaries, nb);
				break;
			}
		}
		workerToSend = xbt_dynar_get_ptr(*workers_array, cpt); 
		MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), (strlen(workerToSend->mailbox) + strlen(MSG_task_get_name(task))) * sizeof(char), workerToSend), toSend->name);
	}
}


void treat_division(msg_task_t task) {
	char mailbox[MAILBOX_SIZE];

	if (xbt_dynar_length(inactive_primaries) > 0) {
		struct primary * p = (struct primary *) malloc(sizeof(struct primary));

		xbt_dynar_pop(inactive_primaries, p);

		if (distributed_strategies == REPUTATIONS) {
			struct reputations_loadBalancing * r_l = MSG_task_get_data(task);
			strcpy(mailbox, r_l->mailbox);
			
			unsigned int cpt;
			struct primary primary;
			
			xbt_dynar_foreach(active_primaries, cpt, primary) {
				if (!strcmp(primary.name, mailbox)) {
					primary.max_reputation = r_l->min_reputation;
					break;
				}
			}
			p->min_reputation = r_l->min_reputation;
			p->max_reputation = r_l->max_reputation;
		}
		else {
			strcpy(mailbox, (char *) MSG_task_get_data(task));
		}

		xbt_dynar_push(active_primaries, p);
		MSG_task_isend(MSG_task_create("ack_division", 0, (strlen("ack_division") + strlen(p->name)) * sizeof(char), p->name), mailbox); 

		if ((able_fusion == -1) && (xbt_dynar_length(active_primaries)) >= 2) {
			unsigned int nb;
			struct primary prim;

			able_fusion = 1;
			xbt_dynar_foreach(active_primaries, nb, prim) {
				MSG_task_isend(MSG_task_create("able_fusion", 0, strlen("able_fusion") * sizeof(char), NULL), prim.name);
			}
			unsigned int nb1;
			struct primary prim1;
			xbt_dynar_foreach(inactive_primaries, nb1, prim1) {
				MSG_task_isend(MSG_task_create("able_fusion", 0, strlen("able_fusion") * sizeof(char), NULL), prim1.name);
			}		
		}	

		if (xbt_dynar_length(inactive_primaries) == 0) {
			unsigned int nb;
			struct primary prim;

			able_division = -1;
			xbt_dynar_foreach(active_primaries, nb, prim) {
				MSG_task_isend(MSG_task_create("unable_division", 0, strlen("unable_division") * sizeof(char), NULL), prim.name);
			}
			unsigned int nb1;
			struct primary prim1;

			xbt_dynar_foreach(inactive_primaries, nb1, prim1) {
				MSG_task_isend(MSG_task_create("unable_division", 0, strlen("unable_division") * sizeof(char), NULL), prim1.name);
			}
		}
	}
	else {
		MSG_task_isend(MSG_task_create("unack_division", 0, strlen("unack_division") * sizeof(char), NULL), MSG_task_get_data(task));
	}
}


void treat_fusion(msg_task_t task) {
	if (xbt_dynar_length(active_primaries) > 1) {
		// we necessarily want at least a primary in the system
		unsigned int cpt;
		struct primary p;
		struct primary * primary = (struct primary *) malloc(sizeof(struct primary)); 

		xbt_dynar_foreach(active_primaries, cpt, p) {
			if (!strcmp(p.name, (char *)MSG_task_get_data(task))) {
				break;
			}
		}
		xbt_dynar_remove_at(active_primaries, cpt, primary);
		xbt_dynar_push(inactive_primaries, primary);

		MSG_task_isend(MSG_task_create("ack_fusion", 0, strlen("ack_fusion") * sizeof(char), NULL), (char *)MSG_task_get_data(task));

		if (xbt_dynar_length(active_primaries) == 1) {
			able_fusion = -1;
			struct primary * toSend = xbt_dynar_get_ptr(active_primaries, 0);
			MSG_task_isend(MSG_task_create("unable_fusion", 0, strlen("unable_fusion") * sizeof(char), NULL), toSend->name);
			
			unsigned int cpt;
			struct primary p;
		
			xbt_dynar_foreach(inactive_primaries, cpt, p) {
				MSG_task_isend(MSG_task_create("unable_fusion", 0, strlen("unable_fusion") * sizeof(char), NULL), p.name);
			}
		}
	}
	else {
		MSG_task_isend(MSG_task_create("unack_fusion", 0, strlen("unack_fusion") * sizeof(char), NULL), (char *)MSG_task_get_data(task));
	}
}


void treat_workers_to_fuse(msg_task_t task) {
	int i;
	struct fusion * recv = (struct fusion *) MSG_task_get_data(task);
	int until = xbt_dynar_length(recv->workersToSend);

	for (i = 0; i < until; i++) {
		struct p_worker * p_w = (struct p_worker *) malloc(sizeof(struct p_worker));
		struct primary * p;

		xbt_dynar_pop(recv->workersToSend, p_w);

		if (distributed_strategies == RANDOM) {
			int nb_rand = rand() % (xbt_dynar_length(active_primaries));
			p = xbt_dynar_get_ptr(active_primaries, nb_rand);			
		}
		else {
			unsigned int cpt;
			struct primary primary;
			char found = -1;
			char min_min = 200;
			char min_max = 200;
			struct primary * p_min_min;
			struct primary * p_min_max;
			char limit[1];
			limit[1] = (recv->max - recv->min) / 2 + recv->min;

			xbt_dynar_foreach(active_primaries, cpt, primary) {
				if ((p_w->reputation >= primary.min_reputation) && (p_w->reputation < primary.max_reputation)) {
					found = 1;
					p = xbt_dynar_get_ptr(active_primaries, cpt);
					break;
				}
				if ((primary.max_reputation <= recv->min) && ((recv->min - primary.max_reputation) < min_min)) {
					min_min = recv->min - primary.max_reputation;
					p_min_min = xbt_dynar_get_ptr(active_primaries, cpt);
				}
				if ((primary.min_reputation >= recv->max) && ((primary.min_reputation - recv->max) < min_max)) {
					min_max = primary.min_reputation - recv->max;
					p_min_max = xbt_dynar_get_ptr(active_primaries, cpt);
				}
			}
			if (found != 1) {
				p_min_min->max_reputation = limit[1];
				p_min_max->min_reputation = limit[1];
				MSG_task_isend(MSG_task_create("limit_max", 0, (strlen("limit_max") + 1)*sizeof(char), limit), p_min_min->name);
				MSG_task_isend(MSG_task_create("limit_min", 0, (strlen("limit_min") + 1)*sizeof(char), limit), p_min_max->name);
			
				unsigned int nb;
				struct primary prim;

				xbt_dynar_foreach(active_primaries, nb, prim) {
					if ((p_w->reputation >= prim.min_reputation) && (p_w->reputation < prim.max_reputation)) {
						p = xbt_dynar_get_ptr(active_primaries, cpt);
						break;
					}
				}
			}
		}

		MSG_task_isend(MSG_task_create("change", MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), p_w), p->name);
	}	
}


int first_primary (int argc, char * argv[]) {
	unsigned long int id;
	char myMailbox[MAILBOX_SIZE];
	msg_task_t task = NULL;
	int nb_clients;
	int nb_finalize = 0;
	int i;

	if (argc != 3) {
		exit(1);
	}

	id = atoi(argv[1]);
	sprintf(myMailbox, "first-primary-%ld", id);
	
	srand(time(NULL) * id + MSG_get_clock());

	active_primaries = xbt_dynar_new(sizeof(struct primary), NULL);
	inactive_primaries = xbt_dynar_new(sizeof(struct primary), NULL);

	struct primary * p0 = (struct primary *) malloc(sizeof(struct primary));
	strcpy(p0->name, "primary-0"); 
	p0->max_reputation = 101;
	p0->min_reputation = 0;

	xbt_dynar_push(active_primaries, p0);

	for (i = 1; i < nb_primaries; i++) {
		struct primary * p = (struct primary *) malloc(sizeof(struct primary));
		sprintf(p->name, "primary-%d", i); 
		p->max_reputation = 101;
		p->min_reputation = 0;

		xbt_dynar_push(inactive_primaries, p);
	}

	nb_clients = atoi(argv[2]);

	while (1) {
		// reception of a message
		MSG_task_receive(&(task), myMailbox);

		/* the primary can receive 4 types of messages: requests from client, 
			finalization from client, join from workers, or answer to tasks of workers */
		if (!strncmp(MSG_task_get_name(task), "finalize", strlen("finalize"))) {
			printf("%s: I receive finalize\n", myMailbox);
			nb_finalize++;	
			if (nb_finalize == nb_clients) {		
				printf("first-primary: forwarding the finalize message\n");	
				// if all clients have finish to send requests, the primary ask the workers to stop
				send_finalize_to_primaries(); 
				break;
			}
		}
		else if (!strncmp(MSG_task_get_name(task), "task", sizeof(char) * strlen("task"))) {
			printf("first-primary: I receive a task\n");
			if (distributed_strategies == RANDOM) {
				send_task_random(task);	
			}
			else {
				send_task_reputations(task);
			}
		}
		else if (!strcmp(MSG_task_get_name(task), "join")) {
			printf("first-primary: I receive a join\n");
			if (distributed_strategies == RANDOM) {
				send_task_random(task);
			}
			else {
				send_join_reputations(task);
			}
		}
		else if (!strcmp(MSG_task_get_name(task), "change")) {
			printf("first-primary: I receive a change, treating it\n");
			treat_change(task);
		}
		else if (!strcmp(MSG_task_get_name(task), "division")) {
			printf("first-primary: I receive a division, treating it\n");
			treat_division(task);
			printf("number of acitve_primaries %ld\n", xbt_dynar_length(active_primaries));
		}
		else if (!strcmp(MSG_task_get_name(task), "fusion")) {
			printf("first-primary: I receive a fusion, treating it\n");
			treat_fusion(task);
		}
		else if (!strcmp(MSG_task_get_name(task), "workers_to_fuse")) {
			printf("first-primary: I receive a workers_to_fuse\n");
			treat_workers_to_fuse(task);
		}
		else {
			// messages incorrect
			printf("%s: I receive an incorrect message\n", myMailbox);
		}
		MSG_task_destroy(task);
		task = NULL;
	}

	MSG_task_destroy(task);
	task = NULL;
	return 0;
}

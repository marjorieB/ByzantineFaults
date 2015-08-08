#include "simulator.h"
#include "first_primary.h"
#include "msg/msg.h"
#include "client.h"
#include "primary.h"


char able_division = 1;

char able_fusion = -1;


void active_primaries_print() {
	struct primary p;
	unsigned int cpt;

	xbt_dynar_foreach (active_primaries, cpt, p) {
 		//printf("p.name= %s, p.max_reputation=%d, p.min_reputation=%d\n", p.name, p.max_reputation, p.min_reputation);
	}
}


void send_finalize_to_primaries () {
	unsigned int cpt;
	struct primary p;
	double complexity = 0.0;
	
	xbt_dynar_foreach(active_primaries, cpt, p) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, p.name);
	}

	//printf("first-primary %ld\n", xbt_dynar_length(inactive_primaries));

	unsigned int nb;
	struct primary primary;

	xbt_dynar_foreach(inactive_primaries, nb, primary) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, primary.name);
	}

	complexity = complexity + xbt_dynar_length(active_primaries) + xbt_dynar_length(inactive_primaries);

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void send_task_random(msg_task_t task) {
	int nb_rand = rand() % (xbt_dynar_length(active_primaries));
	struct primary * p = xbt_dynar_get_ptr(active_primaries, nb_rand);

	if (!strcmp(MSG_task_get_name(task), "join")) {
		//printf("first-primary: I forward the join to primary-%d %s\n", nb_rand, MSG_task_get_name(task));
		MSG_comm_destroy(MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), MSG_task_get_data(task)), p->name));
	}
	else {
		//printf("first-primary: I forward the %s to primary-%d\n", MSG_task_get_name(task), nb_rand);
		MSG_comm_destroy(MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), task_message_size, MSG_task_get_data(task)), p->name));
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", 10.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void send_join_reputations(msg_task_t task) {
	unsigned int cpt;
	struct primary p;
	struct clientDataTask * data = (struct clientDataTask *) malloc(sizeof(struct clientDataTask)); 
	struct primary * pSend;
	double complexity = 0.0;

	data = MSG_task_get_data(task);

	xbt_dynar_foreach(active_primaries, cpt, p) {
		if ((50 >= p.min_reputation) && (50 < p.max_reputation)) {
			pSend = xbt_dynar_get_ptr(active_primaries, cpt);
			complexity += 2.0;
			break;
		}
	}

	MSG_comm_destroy(MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), data), pSend->name));

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 4.0 + cpt * 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void send_task_reputations(msg_task_t task) {
	unsigned int cpt;
	struct primary p;
	struct clientDataTask * data = (struct clientDataTask *) malloc(sizeof(struct clientDataTask)); 
	struct primary * pSend;
	double complexity = 0.0;

	data = MSG_task_get_data(task);

	//printf("first-primary: in send_task_reputations\n");
	//active_primaries_print();

	//printf("data->rangeReputationPrimaryToRequest = %f\n", data->rangeReputationPrimaryToRequest);
	//printf("((double)p.min_reputation / 100.0)) = %f\n", ((double)p.min_reputation / 100.0));
	//printf("(double)p.max_reputation / 100.0) = %f\n",(double)p.max_reputation / 100.0);
	xbt_dynar_foreach(active_primaries, cpt, p) {
		if ((data->rangeReputationPrimaryToRequest >= ((double)p.min_reputation / 100.0)) && (data->rangeReputationPrimaryToRequest < ((double)p.max_reputation / 100.0))) {
			complexity += 2.0;

			//printf("in the boucle, found the good primary\n");
			pSend = xbt_dynar_get_ptr(active_primaries, cpt);
			break;
		}
	}
	//printf("first-primary: I forward the %s to %s\n", MSG_task_get_name(task), pSend->name);

	MSG_comm_destroy(MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), task_message_size, data), pSend->name));

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + cpt * 4.0 + 3.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void treat_change(msg_task_t task) {
	xbt_dynar_t * workers_array = (xbt_dynar_t *) malloc(sizeof(xbt_dynar_t));
	*workers_array = xbt_dynar_new(sizeof(struct p_worker), NULL);
	double complexity = 1.0;


	//printf("active_primaries in the system\n");
	//active_primaries_print();

	//printf("in here\n");
	workers_array = (xbt_dynar_t *)MSG_task_get_data(task);
	//printf("size of the workers_array %ld\n", xbt_dynar_length(*workers_array));

	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach(*workers_array, cpt, p_w) {
		unsigned int nb;
		struct primary p;
		struct p_worker * workerToSend = (struct p_worker *)malloc(sizeof(struct p_worker));
		struct primary * toSend;
		
		//printf("worker treated: name=%s, reputation=%d\n", p_w.mailbox, p_w.reputation);

		xbt_dynar_foreach(active_primaries, nb, p) {
			if ((p_w.reputation >= p.min_reputation) && (p_w.reputation < p.max_reputation)) {
				toSend = xbt_dynar_get_ptr(active_primaries, nb);
				break;
			}
		}
		complexity = complexity + nb * 2.0 + 1.0 + 1.0;	

		workerToSend = xbt_dynar_get_ptr(*workers_array, cpt); 
		MSG_comm_destroy(MSG_task_isend(MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), (strlen(workerToSend->mailbox) + strlen(MSG_task_get_name(task))) * sizeof(char), workerToSend), toSend->name));
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void treat_division(msg_task_t task) {
	char mailbox[MAILBOX_SIZE];
	double complexity = 0.0;
	char * limit_min = (char *) malloc(sizeof(char));
	char * limit_max = (char *) malloc(sizeof(char));


	if (xbt_dynar_length(inactive_primaries) > 0) {
		struct primary * p = (struct primary *) malloc(sizeof(struct primary));
		struct primary * p_divise;

		xbt_dynar_pop(inactive_primaries, p);

		if (distributed_strategies == REPUTATIONS) {
			struct reputations_loadBalancing * r_l = MSG_task_get_data(task);
			strcpy(mailbox, r_l->mailbox);
			
			unsigned int cpt;
			struct primary primary;
			
			xbt_dynar_foreach(active_primaries, cpt, primary) {
				if (!strcmp(primary.name, mailbox)) {
					p_divise = xbt_dynar_get_ptr(active_primaries, cpt);
					p_divise->min_reputation = r_l->max_reputation;
					break;
				}
			}
			p->min_reputation = r_l->min_reputation;
			p->max_reputation = r_l->max_reputation;
			*limit_min = r_l->min_reputation;
			MSG_comm_destroy(MSG_task_isend(MSG_task_create("limit_min", 0, (strlen("limit_min") + 1)*sizeof(char), limit_min), p->name));
			*limit_max = r_l->max_reputation;
			MSG_comm_destroy(MSG_task_isend(MSG_task_create("limit_max", 0, (strlen("limit_max") + 1)*sizeof(char), limit_max), p->name));	
			
			complexity = complexity + 4.0 + cpt + 1.0;
		}
		else {
			strcpy(mailbox, (char *) MSG_task_get_data(task));
			complexity++;
		}

		xbt_dynar_push(active_primaries, p);
		//printf("after division the primaries in the system are:\n");
		//active_primaries_print();

		MSG_comm_destroy(MSG_task_isend(MSG_task_create("ack_division", 0, (strlen("ack_division") + strlen(p->name)) * sizeof(char), p->name), mailbox)); 
	
		complexity += 8.0;

		if ((able_fusion == -1) && (xbt_dynar_length(active_primaries)) >= 2) {
			unsigned int nb;
			struct primary prim;

			able_fusion = 1;
			xbt_dynar_foreach(active_primaries, nb, prim) {
				MSG_comm_destroy(MSG_task_isend(MSG_task_create("able_fusion", 0, strlen("able_fusion") * sizeof(char), NULL), prim.name));
			}
			unsigned int nb1;
			struct primary prim1;
			xbt_dynar_foreach(inactive_primaries, nb1, prim1) {
				MSG_comm_destroy(MSG_task_isend(MSG_task_create("able_fusion", 0, strlen("able_fusion") * sizeof(char), NULL), prim1.name));
			}		

			complexity = complexity + xbt_dynar_length(active_primaries) + xbt_dynar_length(inactive_primaries) + 1.0;
		}	

		if (xbt_dynar_length(inactive_primaries) == 0) {
			unsigned int nb;
			struct primary prim;

			able_division = -1;
			xbt_dynar_foreach(active_primaries, nb, prim) {
				MSG_comm_destroy(MSG_task_isend(MSG_task_create("unable_division", 0, strlen("unable_division") * sizeof(char), NULL), prim.name));
			}
			unsigned int nb1;
			struct primary prim1;

			xbt_dynar_foreach(inactive_primaries, nb1, prim1) {
				MSG_comm_destroy(MSG_task_isend(MSG_task_create("unable_division", 0, strlen("unable_division") * sizeof(char), NULL), prim1.name));
			}
			complexity = complexity + xbt_dynar_length(active_primaries) + xbt_dynar_length(inactive_primaries) + 1.0; 
		}
	}
	else {
		MSG_comm_destroy(MSG_task_isend(MSG_task_create("unack_division", 0, strlen("unack_division") * sizeof(char), NULL), MSG_task_get_data(task)));
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void treat_fusion(msg_task_t task) {
	double complexity = 0.0;

	if (xbt_dynar_length(active_primaries) > 1) {
		// we necessarily want at least a primary in the system
		unsigned int nb;
		struct primary primary;
		unsigned int cpt;
		struct primary p;
		struct primary * primary_active_to_inactive = (struct primary *) malloc(sizeof(struct primary)); 
		char min_min = 120;
		char min_max = 120;
		struct primary * p_min_min = NULL;
		struct primary * p_min_max = NULL;
		char * limit = (char *) malloc(sizeof(char));

		xbt_dynar_foreach(active_primaries, nb, p) {
			if (!strcmp(p.name, (char *)MSG_task_get_data(task))) {
				break;
			}
		}
		xbt_dynar_remove_at(active_primaries, nb, primary_active_to_inactive);
		xbt_dynar_push(inactive_primaries, primary_active_to_inactive);

		xbt_dynar_foreach(active_primaries, cpt, primary) {
			if ((primary.max_reputation <= primary_active_to_inactive->min_reputation) && ((primary_active_to_inactive->min_reputation - primary.max_reputation) < min_min)) {
				//printf("the %s have its max reputation near the min reputation of the fused primary\n", primary.name);
				min_min = primary_active_to_inactive->min_reputation - primary.max_reputation;
				p_min_min = xbt_dynar_get_ptr(active_primaries, cpt);
				complexity += 8.0;
			}
			if ((primary.min_reputation >= primary_active_to_inactive->max_reputation) && ((primary.min_reputation - primary_active_to_inactive->max_reputation) < min_max)) {
				//printf("the %s have its min reputation near the max reputation of the fused primary\n", primary.name);
				min_max = primary.min_reputation - primary_active_to_inactive->max_reputation;
				p_min_max = xbt_dynar_get_ptr(active_primaries, cpt);
				complexity += 11.0;
			}
		}

		if (p_min_min != NULL) {
			complexity++;
			if (p_min_max == NULL) {
				*limit = primary_active_to_inactive->max_reputation;
				p_min_min->max_reputation = primary_active_to_inactive->max_reputation;
				//printf("we change the limit max of p_min_min to %d\n", *limit);
				complexity += 3.0;
			}
			else {
				//printf("we change the limit max of p_min_min to %d\n", *limit);
				p_min_min->max_reputation = (primary_active_to_inactive->max_reputation - primary_active_to_inactive->min_reputation) / 2 + primary_active_to_inactive->min_reputation;
				complexity += 5.0;
			}
			MSG_comm_destroy(MSG_task_isend(MSG_task_create("limit_max", 0, (strlen("limit_max") + 1)*sizeof(char), limit), p_min_min->name));
		}
		if (p_min_max != NULL) {
			complexity++;
			if (p_min_min == NULL) {
				*limit = primary_active_to_inactive->min_reputation;
				p_min_max->min_reputation = primary_active_to_inactive->min_reputation;
				complexity += 3.0;
			}
			else {
				p_min_max->min_reputation = (primary_active_to_inactive->max_reputation - primary_active_to_inactive->min_reputation) / 2 + primary_active_to_inactive->min_reputation;
				complexity += 5.0;
			}					
			MSG_comm_destroy(MSG_task_isend(MSG_task_create("limit_min", 0, (strlen("limit_min") + 1)*sizeof(char), limit), p_min_max->name));
		}

		MSG_comm_destroy(MSG_task_isend(MSG_task_create("ack_fusion", 0, strlen("ack_fusion") * sizeof(char), NULL), (char *)MSG_task_get_data(task)));

		complexity = complexity + cpt + 4.0;

		if (xbt_dynar_length(active_primaries) == 1) {
			able_fusion = -1;
			struct primary * toSend = xbt_dynar_get_ptr(active_primaries, 0);
			MSG_comm_destroy(MSG_task_isend(MSG_task_create("unable_fusion", 0, strlen("unable_fusion") * sizeof(char), NULL), toSend->name));
			
			unsigned int nb;
			struct primary p;
		
			xbt_dynar_foreach(inactive_primaries, nb, p) {
				MSG_comm_destroy(MSG_task_isend(MSG_task_create("unable_fusion", 0, strlen("unable_fusion") * sizeof(char), NULL), p.name));
			}
			complexity = 2.0 + nb;
		}
	}
	else {
		MSG_comm_destroy(MSG_task_isend(MSG_task_create("unack_fusion", 0, strlen("unack_fusion") * sizeof(char), NULL), (char *)MSG_task_get_data(task)));
	}
	
	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void treat_workers_to_fuse(msg_task_t task) {
	int i;
	struct fusion * recv = (struct fusion *) MSG_task_get_data(task);
	int until = xbt_dynar_length(recv->workersToSend);
	int until_tasks = xbt_fifo_size(recv->tasks);
	double complexity = 2.0;
	//char * min = (char *) malloc(sizeof(char));
	//char * max = (char *) malloc(sizeof(char));


	//active_primaries_print();

	for (i = 0; i < until; i++) {
		struct p_worker * p_w = (struct p_worker *) malloc(sizeof(struct p_worker));
		struct primary * p;

		xbt_dynar_pop(recv->workersToSend, p_w);

		complexity += 2.0;

		if (distributed_strategies == RANDOM) {
			int nb_rand = rand() % (xbt_dynar_length(active_primaries));
			p = xbt_dynar_get_ptr(active_primaries, nb_rand);		
			complexity += 5.0;	
		}
		else {
			unsigned int cpt;
			struct primary primary;
			/*char found = -1;
			char min_min = 120;
			char min_max = 120;
			struct primary * p_min_min = NULL;
			struct primary * p_min_max = NULL;
			char * limit = (char *) malloc(sizeof(char));
			*limit = (recv->max - recv->min) / 2 + recv->min;*/

			complexity += 3.0;
			xbt_dynar_foreach(active_primaries, cpt, primary) {
				//printf("recv->min = %d, and recv->max=%d\n", recv->min, recv->max);
				//printf("primary.reputation_min=%d, and primary.reputation_max=%d\n", primary.min_reputation, primary.max_reputation);

				if ((p_w->reputation >= primary.min_reputation) && (p_w->reputation < primary.max_reputation)) {
					//found = 1;
					//printf("I found a primary to take the worker in charge\n");
					p = xbt_dynar_get_ptr(active_primaries, cpt);
					complexity += 4.0;
					break;
				}
				/*if ((primary.max_reputation <= recv->min) && ((recv->min - primary.max_reputation) < min_min)) {
					//printf("the %s have its max reputation near the min reputation of the fused primary\n", primary.name);
					min_min = recv->min - primary.max_reputation;
					p_min_min = xbt_dynar_get_ptr(active_primaries, cpt);
					complexity += 8.0;
				}
				if ((primary.min_reputation >= recv->max) && ((primary.min_reputation - recv->max) < min_max)) {
					//printf("the %s have its min reputation near the max reputation of the fused primary\n", primary.name);
					min_max = primary.min_reputation - recv->max;
					p_min_max = xbt_dynar_get_ptr(active_primaries, cpt);
					complexity += 11.0;
				}*/
			}
			/*complexity++;
			if (found != 1) {
				//printf("we didn't found an appropriate primary\n");
				if (p_min_min != NULL) {
					if (p_min_max == NULL) {
						*limit = recv->max;
						*max = recv->max;
						p_min_min->max_reputation = *max;
						//printf("we change the limit max of p_min_min to %d\n", *limit);
					}
					else {
						*min = (recv->max - recv->min) / 2 + recv->min;
						//printf("we change the limit max of p_min_min to %d\n", *limit);
						p_min_min->max_reputation = *min;
					}
					MSG_comm_destroy(MSG_task_isend(MSG_task_create("limit_max", 0, (strlen("limit_max") + 1)*sizeof(char), limit), p_min_min->name));
				}
				if (p_min_max != NULL) {
					if (p_min_min == NULL) {
						*limit = recv->min;
						*min = recv->min;
						p_min_max->min_reputation = *min;
					}
					else {
						*min = (recv->max - recv->min) / 2 + recv->min;
						p_min_max->min_reputation = *min;
					}					
					MSG_comm_destroy(MSG_task_isend(MSG_task_create("limit_min", 0, (strlen("limit_min") + 1)*sizeof(char), limit), p_min_max->name));
				}
				//printf("AFFFFFFFFFFFFFFFFFFTER THE CHANGE\n");
				//active_primaries_print();

				unsigned int nb;
				struct primary prim;

				xbt_dynar_foreach(active_primaries, nb, prim) {
					if ((p_w->reputation >= prim.min_reputation) && (p_w->reputation < prim.max_reputation)) {
						//printf("now we found a good primary to the worker with reputation %d\n", p_w->reputation);
						p = xbt_dynar_get_ptr(active_primaries, nb);
						break;
					}
				}
				complexity = complexity + 1.0 + nb * 2.0;
			}*/
		}

		//printf("send the change of worker to %s handling reputations between %d and %d\n", p->name, p->min_reputation, p->max_reputation);
		MSG_comm_destroy(MSG_task_isend(MSG_task_create("change", MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), p_w), p->name));
	}	
	//printf("BEFOOOOOOOOOOOOOOOOOOOOORE THE TASKS\n");
	//active_primaries_print();


	for (i = 0; i < until_tasks; i++) {
		msg_task_t * tmp = xbt_fifo_pop(recv->tasks);
		//printf("the name of the task to transmit is (see next line): \n");
		//printf("MSG_task_name(*tmp)=%s\n", (char *)MSG_task_get_name(*tmp));
		msg_task_t task = MSG_task_create(MSG_task_get_name(*tmp), MSG_task_get_compute_duration(*tmp), MSG_task_get_data_size(*tmp), MSG_task_get_data(*tmp));

		if (distributed_strategies == RANDOM) {
			//printf("first-primary-0: transmit a task\n");
			send_task_random(task);	
		}
		else {
			//printf("first-primary-0: transmit a task\n");
			send_task_reputations(task);
			//printf("after the transmition\n");
		}
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 2.0 + until_tasks, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


int first_primary (int argc, char * argv[]) {
	//printf("first-primary start\n");
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
			//printf("%s: I receive finalize\n", myMailbox);
			nb_finalize++;	
			if (nb_finalize == nb_clients) {		
				//printf("first-primary: forwarding the finalize message\n");	
				// if all clients have finish to send requests, the primary ask the workers to stop
				send_finalize_to_primaries(); 
				break;
			}
		}
		else if (!strncmp(MSG_task_get_name(task), "task", sizeof(char) * strlen("task"))) {
			//printf("first-primary: I receive a task\n");
			if (distributed_strategies == RANDOM) {
				send_task_random(task);	
			}
			else {
				send_task_reputations(task);
			}
		}
		else if (!strcmp(MSG_task_get_name(task), "join")) {
			//printf("first-primary: I receive a join\n");
			if (distributed_strategies == RANDOM) {
				send_task_random(task);
			}
			else {
				send_join_reputations(task);
			}
		}
		else if (!strcmp(MSG_task_get_name(task), "change")) {
			//printf("first-primary: I receive a change, treating it\n");
			treat_change(task);
		}
		else if (!strcmp(MSG_task_get_name(task), "division")) {
			//printf("first-primary: I receive a division, treating it\n");
			treat_division(task);
			//printf("number of acitve_primaries %ld\n", xbt_dynar_length(active_primaries));
		}
		else if (!strcmp(MSG_task_get_name(task), "fusion")) {
			//printf("first-primary: I receive a fusion, treating it\n");
			treat_fusion(task);
		}
		else if (!strcmp(MSG_task_get_name(task), "workers_to_fuse")) {
			//printf("first-primary: I receive a workers_to_fuse\n");
			treat_workers_to_fuse(task);
		}
		else {
			// messages incorrect
			//printf("%s: I receive an incorrect message\n", myMailbox);
		}
		MSG_task_destroy(task);
		task = NULL;
	}

	MSG_task_destroy(task);
	task = NULL;
	return 0;
}

#include "msg/msg.h"
#include "worker.h"
#include "task.h"
#include "simulator.h"
#include <time.h>


void ask_to_join(char * mailbox, char * myMailbox) {
	msg_task_t join = MSG_task_create("join", 0, sizeof(char) * (strlen(myMailbox) + strlen("join")), myMailbox);
	MSG_task_isend(join, mailbox);
}


void receive_ack(struct worker * worker, char * myMailbox) {
	_XBT_GNUC_UNUSED int res;
	msg_task_t ack = NULL;

	res = MSG_task_receive(&(ack), myMailbox);
	xbt_assert(res == MSG_OK, "MSG_task_receive failed on worker");

	strcpy(worker->primary, (char *)MSG_task_get_data(ack));
	printf("%s: receive a ack from %s\n", myMailbox, worker->primary);

	MSG_task_destroy(ack);
}


//this function returns the time the process has to wait before entering in the system
double enter_the_system (struct worker * me) {
	struct present_or_failed p_o_f;
	unsigned int cpt;

	xbt_dynar_foreach (presence[me->index], cpt, p_o_f) {
		if (p_o_f.type == 1) {
			return p_o_f.time - time_start;
		}
	}

	MSG_process_kill(MSG_process_self());
	return 0.0;
}


// this function returns 0 if the node is present in the system, otherwise this function returns the time where the node have to recover (ask the primary to enter in the system)
double present(struct worker * me, double duration_task) {
	printf("start of function present\n");
	double previous_time_start_event;
	int previous_type_event;
	double time_crash = 0.0;
	char crash = 0;


	struct present_or_failed p_o_f;
	unsigned int cpt;

	xbt_dynar_foreach (presence[me->index], cpt, p_o_f) {
		if (p_o_f.time >= time_start + MSG_get_clock()) {	
			if (p_o_f.time == time_start + MSG_get_clock()) {
				// the time in the trace corresponds to the time in the simulation, we have to check if the node is present at this time
				if (p_o_f.type == 0) {
					if (crash != 1) {
						crash = 1;
						time_crash = p_o_f.time;
					}
				}				
				else {
					if (previous_type_event == 0) {
						if (crash != 1) {
							crash = 1;
							time_crash = previous_time_start_event;
						}
					}
				}
			}
			if (p_o_f.time > time_start + MSG_get_clock() + duration_task) {
				break;
			}
		}
		previous_time_start_event = p_o_f.time;
		previous_type_event = p_o_f.type;
	}

	if (crash == 0) {
		return 0.0;
	}
	printf("here\n");

	struct present_or_failed p;
	unsigned int i;

	xbt_dynar_foreach (presence[me->index], i, p) {
		if (p.time > time_crash) {
			if (p.type == 1) {
				return p.time - (MSG_get_clock() + time_start);
			}
		}
	}
	exit(1);
}


double treat_task_worker(struct worker * me, msg_task_t task, char * myMailbox) {
	msg_task_t answer;	
	struct w_task * data_toSend = (struct w_task *) malloc(sizeof(struct w_task));
	double time_to_wait;

	if ((time_to_wait = present(me, (MSG_task_get_compute_duration(task) / MSG_get_host_power_peak_at(MSG_host_self(), 0)))) == 0.0) {

		printf("after present\n");	
		if (me->reputation == 1) {
			data_toSend->answer = GOOD_ANSWER;
		}
		else if (me->reputation == -1) {
			data_toSend->answer = rand() % BAD_ANSWER;
		}
		else {
			if ((rand() % 100) < 50) {
				data_toSend->answer = rand() % BAD_ANSWER;
			}
			else {
				data_toSend->answer = GOOD_ANSWER;
			} 
		}
		MSG_task_execute(task);	
	}	
	printf("after present\n");	
	strcpy(data_toSend->client, MSG_task_get_data(task));
	strcpy(data_toSend->worker_name, myMailbox);
	strcpy(data_toSend->task_name, MSG_task_get_name(task));

	if (time_to_wait == 0.0) {
		answer = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, data_toSend);
	}
	else if (time_to_wait > 0.0) {
		printf("worker %ld send crash\n", me->id);
		answer = MSG_task_create("crash", 0, 0, data_toSend);
	}
	else {
		printf("error in the function \"present\"\n");
	}

	MSG_task_isend(answer, me->primary);

	return time_to_wait;
}


int worker (int argc, char * argv[]) {
	char myMailbox[MAILBOX_SIZE];
	struct worker * me = (struct worker *) malloc(sizeof(struct worker));
	char primary[MAILBOX_SIZE];
	double time_to_wait;

	if (argc != 5) {
		exit(1);
	}
//	printf("worker : before reading\n");

	// the worker ask to join the system, then wait for an acknowledgement from the primary and then wait for request to treat
	me->id = atoi(argv[1]);	
	sprintf(myMailbox, "worker-%ld", me->id);
	me->index = atoi(argv[2]);
	strcpy(primary, argv[3]);
	me->reputation = atoi(argv[4]);

	srand(time(NULL) * me->id + MSG_get_clock());

//	printf("worker : after reading\n");

	if (me->reputation == 0) {
		printf("%s: I am an average node\n", myMailbox);
	}
	else if (me->reputation == -1) {
		printf("%s: I am a byzantine node\n", myMailbox);
	}
	else {
		printf("%s: I am a reliable node\n", myMailbox);
	}


	// wait the time indicated in the trace to enter the system	
	MSG_process_sleep(enter_the_system(me));
	printf("end enter_the_system\n");


	printf("%s: ask to join the system to %s\n", myMailbox, primary);
	ask_to_join(primary, myMailbox);
	receive_ack(me, myMailbox);

	while (1) {
		_XBT_GNUC_UNUSED int res;
		msg_task_t task = NULL;

		res = MSG_task_receive(&(task), myMailbox);
		xbt_assert(res == MSG_OK, "MSG_task_receive failed on worker");

		if (!strcmp(MSG_task_get_name(task), "ack")) {
			strcpy(me->primary, (char *)MSG_task_get_data(task));
			MSG_task_destroy(task);
			task = NULL;
		}
		else if (!strcmp(MSG_task_get_name(task), "finalize")) {
			printf("%s: I receive finalize %f\n", myMailbox, MSG_get_clock());
			MSG_task_destroy(task);
			task = NULL;
			break;
		}
		else if (!strncmp(MSG_task_get_name(task), "task", strlen("task") * sizeof(char))) {
			// if the availability_file still don't work, do a random to know if the worker will answer and put a percentage of availability in the xml file to describe the node
			printf("reception of a task %s\n", myMailbox);
			time_to_wait = treat_task_worker(me, task, myMailbox);
			MSG_task_destroy(task);
			task = NULL;
			if (time_to_wait == -1) {
				printf("error in function \"present\"\n");
			}
			else if (time_to_wait != 0.0) {
				// wait until the time where the node recover
				MSG_process_sleep(time_to_wait);
				if (centrality == DISTRIBUTED) {
					// we have to contact the first-primary to join again the system
					strcpy(primary, argv[3]);
				}
				ask_to_join(primary, myMailbox);
				receive_ack(me, myMailbox);
			}
		}	
		else if (!strncmp(MSG_task_get_name(task), "ejected", strlen("ejected") * sizeof(char))) {
			ask_to_join(primary, myMailbox);
			receive_ack(me, myMailbox);
			MSG_task_destroy(task);
			task = NULL;
		}
	/*	else if (!strncmp(MSG_task_get_name(task), "ackchange", strlen("ackchange") * sizeof(char))) {
			strcpy(me->primary, (char *)MSG_task_get_data(task));
			printf("%s: I receive a ackchange from %s\n", myMailbox, me->primary);
			MSG_task_destroy(task);
			task = NULL;
		} */
		else {
			// message incorrect
			printf("%s message incorrect\n", myMailbox);
			MSG_task_destroy(task);
			task = NULL;
		}
	}	
	
	free(me);

	return 0;
}










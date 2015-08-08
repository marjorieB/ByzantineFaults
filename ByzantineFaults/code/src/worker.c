#include "msg/msg.h"
#include "worker.h"
#include "task.h"
#include "simulator.h"
#include <time.h>


void ask_to_join(char * mailbox, char * myMailbox) {
	msg_task_t join = MSG_task_create("join", 0, sizeof(char) * (strlen(myMailbox) + strlen("join")), myMailbox);
	MSG_task_send(join, mailbox);
}


void receive_ack(struct worker * worker, char * myMailbox) {
	_XBT_GNUC_UNUSED int res;
	msg_task_t ack = NULL;

	res = MSG_task_receive(&(ack), myMailbox);
	xbt_assert(res == MSG_OK, "MSG_task_receive failed on worker");

	strcpy(worker->primary, (char *)MSG_task_get_data(ack));
	//printf("%s: receive a ack from %s\n", myMailbox, worker->primary);

	MSG_task_destroy(ack);
}


int treat_task_worker(struct worker * me, msg_task_t task, char * myMailbox) {
	msg_task_t answer;	
	struct w_task * data_toSend = (struct w_task *) malloc(sizeof(struct w_task));

	strcpy(data_toSend->client, MSG_task_get_data(task));
	strcpy(data_toSend->worker_name, myMailbox);
	strcpy(data_toSend->task_name, MSG_task_get_name(task));

	if ((rand() % 100) < 2) {
		//printf("%s: I crashed\n", myMailbox);
		data_toSend->answer = BAD_ANSWER + 1;
		answer = MSG_task_create("crash", 0, strlen("crash") * sizeof(char) + sizeof(data_toSend), data_toSend);
		MSG_task_send(answer, me->primary);
		return 1;
	}

	if (me->reputation == 1) {
		if ((rand() % 100) < 92) {
			data_toSend->answer = GOOD_ANSWER;
		}
		else {
			data_toSend->answer = rand() % BAD_ANSWER;
		} 
	}
	else if (me->reputation == -1) {
		if ((rand() % 100) < 92) {
			data_toSend->answer = rand() % BAD_ANSWER;
		}
		else {
			data_toSend->answer = GOOD_ANSWER;
		} 
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

	answer = MSG_task_create("answer", 0, strlen("answer") * sizeof(char) + sizeof(data_toSend), data_toSend);

	MSG_task_send(answer, me->primary);

	return -1;
}




int worker (int argc, char * argv[]) {
	//printf("worker start\n");
	char myMailbox[MAILBOX_SIZE];
	struct worker * me = (struct worker *) malloc(sizeof(struct worker));
	char primary[MAILBOX_SIZE];
	//double time_to_wait;
	int ret;

	if (argc != 4) {
		//printf("here in the workers\n");
		exit(1);
	}
//	//printf("worker : before reading\n");

	// the worker ask to join the system, then wait for an acknowledgement from the primary and then wait for request to treat
	me->id = atoi(argv[1]);	
	sprintf(myMailbox, "worker-%ld", me->id);
	//me->index = atoi(argv[2]);
	strcpy(primary, argv[2]);
	me->reputation = atoi(argv[3]);

	srand(time(NULL) * me->id + MSG_get_clock());

	// wait the time indicated in the trace to enter the system	
	//MSG_process_sleep(enter_the_system(me));
	MSG_process_sleep(((double)(rand () % 100001)) / 1000.0);


	//printf("%s: ask to join the system to %s\n", myMailbox, primary);
	ask_to_join(primary, myMailbox);
	receive_ack(me, myMailbox);

	while (1) {
		_XBT_GNUC_UNUSED int res;
		msg_task_t task = NULL;

		res = MSG_task_receive(&(task), myMailbox);
		xbt_assert(res == MSG_OK, "MSG_task_receive failed on worker");

		if (!strcmp(MSG_task_get_name(task), "ack")) {
			//printf("%s: I receive an ack\n", myMailbox);
			strcpy(me->primary, (char *)MSG_task_get_data(task));
			MSG_task_destroy(task);
			task = NULL;
		}
		else if (!strcmp(MSG_task_get_name(task), "finalize")) {
			//printf("%s: I receive finalize %f\n", myMailbox, MSG_get_clock());
			MSG_task_destroy(task);
			task = NULL;
			break;
		}
		else if (!strncmp(MSG_task_get_name(task), "task", strlen("task") * sizeof(char))) {
			// if the availability_file still don't work, do a random to know if the worker will answer and put a percentage of availability in the xml file to describe the node
			//printf("reception of a task %s\n", myMailbox);
			ret = treat_task_worker(me, task, myMailbox);
			MSG_task_destroy(task);
			task = NULL;
			if (ret == 1) {
				MSG_process_sleep(((double)(rand () % 1000001)) / 1000.0);
				if (centrality == DISTRIBUTED) {
					// we have to contact the first-primary to join again the system
					strcpy(primary, argv[3]);
				}
				ask_to_join(primary, myMailbox);
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
			//printf("%s: I receive a ackchange from %s\n", myMailbox, me->primary);
			MSG_task_destroy(task);
			task = NULL;
		} */
		else {
			// message incorrect
			//printf("%s message incorrect\n", myMailbox);
			MSG_task_destroy(task);
			task = NULL;
		}
	}	
	
	free(me);

	return 0;
}










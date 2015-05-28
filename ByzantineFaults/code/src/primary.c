#include "msg/msg.h"
#include "task.h"
#include "primary.h"
#include "reputation_strategy.h"
#include <string.h>


void dynar_print() {
	struct p_worker p_w;
	unsigned int cpt;

	xbt_dynar_foreach (workers, cpt, p_w) {
 		printf("p_w.name= %s\np_w.reputation=%d\n", p_w.mailbox, p_w.reputation);
	}
}


// this function permits to find if an element of the dynamic array (passed in argument) has as identifier the name in argument
int dynar_search(const char * name) {
	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach (workers, cpt, p_w) {
 		if (!strcmp(p_w.mailbox, name)) {
			return cpt;
		}
	}
	printf("non trouvé\n");
	return -1;
}


void send_finalize_to_workers() {
	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach (workers, cpt, p_w) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, p_w.mailbox);
	}	

	// de même pour les workers dans les groupes
}


void add_new_worker(const char * name, char * myMailbox) {
	int index;
	msg_task_t ack;
	struct p_worker worker;

	printf("I'm adding a worker\n");

	// we must verify that it isn't a worker we already have, if it is the case, we need to reset it (the node has been shutdown and the primary didn't seen it.
	if ((index = dynar_search(name)) != -1) {
		xbt_dynar_remove_at(workers, index, NULL);
	}
	
	// creation of a new worker
	strcpy(worker.mailbox, name);
	worker.totR = 0;
	worker.totC = 0;
	updateReputation_Sonnek(&worker);
	printf("valeur de la réputation: %d\n", worker.reputation);

	xbt_dynar_push(workers, &worker);
	
	printf("sending acknoledgeement to the worker\n");
	ack = MSG_task_create("ack", ACK_COMPUTE_DURATION, ACK_MESSAGE_SIZE, myMailbox);
	MSG_task_send(ack, name);	
}


void put_task_fifo(msg_task_t task) {

xbt_fifo_push(tasks, &task);
}


void treat_answer() {

}


int primary (int argc, char * argv[]) {
	unsigned long int id;
	int nb_clients;
	char myMailbox[MAILBOX_SIZE];
	//char mailbox_recv[MAILBOX_SIZE];
	int nb_finalize = 0;
	msg_task_t task_todo = NULL;
	unsigned char can_treat_task = -1;

	if (argc != 3) {
		exit(1);
	}

	workers = xbt_dynar_new(sizeof(struct p_worker), NULL);
	tasks = xbt_fifo_new();

	id = atoi(argv[1]);
	sprintf(myMailbox, "primary-%ld", id);
	nb_clients = atoi(argv[2]);

	while (1) {
		printf("primary: waiting for message\n");
		// reception of a message
		MSG_task_receive(&(task_todo), myMailbox);

		/* the primary can receive 4 types of messages: requests from client, 
			finalization from client, join from workers, or answer to tasks of workers */
		if (!strcmp(MSG_task_get_name(task_todo), "finalize")) {
			printf("primary: reception de finalize\n");
			MSG_task_destroy(task_todo);
			task_todo = NULL;
			nb_finalize++;
			if (nb_finalize == nb_clients) {
				// if all clients have finish to send requests, the primary ask the workers to stop
				send_finalize_to_workers(); 
				break;
			}
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "task", sizeof(char) * strlen("task"))) {
			
			printf("%s: I receive a request from %s\n", myMailbox, (char *)MSG_task_get_data(task_todo));
			// the primary just receive a task to execute
			if (can_treat_task == -1) {////////////////////////////////////// a remplacer par est-ce qu'il y a des groupes de formés?
				// there isn't group of workers, the primary will treat the tasks as soon as there is at least a group to execute the requests
				printf("%s: I can't treat the request now, I put it in a fifo\n", myMailbox);
				put_task_fifo(task_todo);
				MSG_task_destroy(task_todo);
				task_todo = NULL;
			}	
			else {
				// we treat the 
				//treat_tasks(groups, tasks);		
				printf("I treat the task\n");		

			}		
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "join", sizeof(char) * strlen("join"))) {

			printf("receive a joining request from %s\n", (char *)MSG_task_get_data(task_todo));
			// a worker want to join the system			
			add_new_worker((char *)MSG_task_get_data(task_todo), myMailbox);
			dynar_print();
			MSG_task_destroy(task_todo);
			task_todo = NULL;
			if (xbt_dynar_length(workers) >= MIN_WORKERS_GROUP) {
				printf("je dois former des groupes\n");
				//form_group(workers);
				if (xbt_fifo_size(tasks) != 0) {
					//treat_tasks(groups, tasks);
					printf("je dois traiter des tâches\n");
				}
			}
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "answer", sizeof(char) * strlen("answer"))) {
			// the primary receive an answer to a request from a worker
			treat_answer();
		}
		else {
			// messages incorrect
		
		}
	}

	return 0;
}


























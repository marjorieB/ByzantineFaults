#include "msg/msg.h"
#include "task.h"
#include "primary.h"
#include "reputation_strategy.h"
#include "group_formation_strategy.h"
#include "fifo_private.h"
#include <string.h>


void tasks_print() {
	xbt_fifo_item_t i;
	msg_task_t * n;

	printf("content of the list \"tasks\":\n");
   for(i = xbt_fifo_get_first_item(tasks); ((i) ? (n = (msg_task_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		printf("client: %s, task: %s\n", (char *)MSG_task_get_data(*n), MSG_task_get_name(*n));
	}
}


void processing_tasks_print() {
	xbt_fifo_item_t i;
	struct p_task * n;

	printf("content of the list \"processing_tasks\":\n");
   for(i = xbt_fifo_get_first_item(processing_tasks); ((i) ? (n = (struct p_task *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		printf("client: %s, task: %s\n", n->client, n->task_name);
	}
}


void workers_print() {
	struct p_worker p_w;
	unsigned int cpt;

	printf("list of the workers of the system\n");
	xbt_dynar_foreach (workers, cpt, p_w) {
 		printf("p_w.name= %s, p_w.reputation=%d\n", p_w.mailbox, p_w.reputation);
	}
}


void groups_print(xbt_fifo_t * f) {
	xbt_fifo_item_t i;
	xbt_dynar_t * n;
	int nb = 0;

   for(i = xbt_fifo_get_first_item(*f); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		nb++;
		printf("group number %d\n", nb);
		
		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*n, cpt, p_w) {
 			printf("p_w.name= %s, p_w.reputation=%d\n", p_w.mailbox, p_w.reputation);
		}
	}
}


// this function permits to find if an element of the dynamic array (passed in argument) has as identifier the name in argument
struct p_worker * dynar_search(const char * name) {
	unsigned int cpt;
	struct p_worker * p_w = (struct p_worker *) malloc(sizeof(struct p_worker));

	xbt_dynar_foreach (workers, cpt, *p_w) {
 		if (!strcmp(p_w->mailbox, name)) {
			return p_w;
		}
	}
	return NULL;
}


void send_finalize_to_workers() {
	unsigned int cpt;
	struct p_worker p_w;
	xbt_fifo_item_t i;
	xbt_dynar_t * n;

	// at the end the workers can be in the workers list or in the inactive_groups. But as their is no more tasks from client, it is impossible to have worker in the active groups
	xbt_dynar_foreach (workers, cpt, p_w) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, p_w.mailbox);
	}	

   for(i = xbt_fifo_get_first_item(inactive_groups); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {		
		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*n, cpt, p_w) {
 			msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
			MSG_task_send(finalize, p_w.mailbox);
		}
	}
}


void add_new_worker(const char * name, char * myMailbox) {
	struct p_worker * w_found = (struct p_worker *) malloc(sizeof(struct p_worker));
	msg_task_t ack;
	struct p_worker * worker = (struct p_worker *) malloc(sizeof(struct p_worker));

	// we must verify that it isn't a worker we already have, if it is the case, we need to reset it (the node has been shutdown and the primary didn't seen it.
	if ((w_found = dynar_search(name)) != NULL) {
		w_found->totR = 0;
		w_found->totC = 0;
		updateReputation_Sonnek(w_found);
	}
	else {
		// creation of a new worker
		strcpy(worker->mailbox, name);
		worker->totR = 0;
		worker->totC = 0;
		updateReputation_Sonnek(worker);
		printf("valeur de la réputation: %d\n", worker->reputation);

		xbt_dynar_push(workers, worker);
	}
	printf("sending acknowledgement to the worker\n");
	ack = MSG_task_create("ack", ACK_COMPUTE_DURATION, ACK_MESSAGE_SIZE, myMailbox);
	MSG_task_send(ack, name);	
}


void put_task_fifo(msg_task_t task) {
	msg_task_t * todo = (msg_task_t *) malloc(sizeof(msg_task_t));

	*todo = MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), MSG_task_get_data(task));
	xbt_fifo_push(tasks, todo);
}


xbt_fifo_item_t fifo_supress_item_head(xbt_fifo_t l) {
	xbt_fifo_item_t item;

	if (l->head == NULL) {
		return NULL;
	}
	item = l->head;

	l->head = item->next;
	if (l->head == NULL) {
		l->tail = NULL;
	}
	else {
		l->head->prev = NULL;
	}

	(l->count)--;
	item->next = NULL;

	return item;
}


void * fifo_supress_head(xbt_fifo_t l) {
	xbt_fifo_item_t item;
	void * content;

	if (l == NULL) {
		return NULL;
	}
	if (!(item = fifo_supress_item_head(l))) {
		return NULL;
	}
	
	content = item->content;
	xbt_fifo_free_item(item);
	return content;
}

void treat_tasks() {
	// travel the list of inactive groups and distribute them the tasks in the list tasks
	// put the group that have a task to the list of active group and put the tasks in a list of processing task
	xbt_fifo_item_t i;
	xbt_dynar_t * n;
	int k;
	struct p_worker p_w;
	unsigned int cpt;
	int nb = 0;

   for(i = xbt_fifo_get_first_item(inactive_groups); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		if (xbt_fifo_size(tasks) != 0) {
			struct p_task * t = (struct p_task *) malloc(sizeof(struct p_task));
			msg_task_t * to_process = (msg_task_t *) malloc(sizeof(msg_task_t));

			to_process = fifo_supress_head(tasks);		
			strcpy(t->client, (char *)MSG_task_get_data(*to_process));
			strcpy(t->task_name, MSG_task_get_name(*to_process));
			t->w_answers = xbt_dynar_new(sizeof(struct p_answer_worker), NULL);
			t->duration = MSG_task_get_compute_duration(*to_process);
			t->size = MSG_task_get_data_size(*to_process);
			t->final_answer = 0;
			t->nb_forwarded = xbt_dynar_length(*n);
			t->nb_crashed = 0;
			t->nb_answers_received = 0;
			t->nb_false_answers = 0;
			t->nb_correct_answers = 0;

			nb++;

			xbt_dynar_foreach (*n, cpt, p_w) {
				printf("send task %s %s to %s\n", t->client, t->task_name, p_w.mailbox);  
	 			msg_task_t to_send = MSG_task_create(MSG_task_get_name(*to_process), MSG_task_get_compute_duration(*to_process), MSG_task_get_data_size(*to_process), MSG_task_get_data(*to_process));
				MSG_task_send(to_send, p_w.mailbox);
			}

			xbt_fifo_push(processing_tasks, t);
			cpt++;
		}
		else {
			break;
		}
	}
	printf("valeur de cpt=%d\n", cpt);
	printf("avant transfert de groupes\n");
	printf("content of the inactive groups\n");
	groups_print(&inactive_groups);
	printf("content of the active groups\n");
	groups_print(&active_groups);
	for (k = 0; k < nb; k++) {
		xbt_fifo_push(active_groups, (xbt_dynar_t *)xbt_fifo_get_item_content(xbt_fifo_get_first_item(inactive_groups)));
		xbt_fifo_remove(inactive_groups, (xbt_dynar_t *)xbt_fifo_get_item_content(xbt_fifo_get_first_item(inactive_groups)));
	}
	tasks_print();
	processing_tasks_print();
	printf("content of the inactive groups\n");
	groups_print(&inactive_groups);
	printf("content of the active groups\n");
	groups_print(&active_groups);
}


void try_to_treat_tasks() {
	printf("TRY_TO_TREAT_TASKS: content of the inactive groups\n");
	groups_print(&inactive_groups);
	printf("TRY_TO_TREAT_TASKS: number of available workers=%ld\n", xbt_dynar_length(workers));
	if ((xbt_fifo_size(inactive_groups) != 0) || (xbt_dynar_length(workers) >= NB_MIN_GROUP)) {
		if (xbt_dynar_length(workers) >= NB_MIN_GROUP) {
			formGroup_fixed_fit();
			printf("content of the inactive groups\n");
			groups_print(&inactive_groups);
		}
		printf("TRY_TO_TREAT_TASKS: it is possible to treat tasks-> go processing\n");
		treat_tasks();
		tasks_print();
	}	
}


struct p_worker * give_worker_dynar(char * name) {
	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach(workers, cpt, p_w) {
 		if (!strcmp(p_w.mailbox, name)) {	
			return xbt_dynar_get_ptr(workers, cpt);
		}
	}
	return NULL;
}


struct p_worker * give_worker_active_groups(char * name) {
	xbt_fifo_item_t i;
	xbt_dynar_t * n;

	for(i = xbt_fifo_get_first_item(active_groups); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
	
		struct p_worker p_w;		
		unsigned int cpt;

		xbt_dynar_foreach(*n, cpt, p_w) { 
	 		if (!strcmp(p_w.mailbox, name)) {
				return xbt_dynar_get_ptr(*n, cpt);
			}
		}
	}
	return NULL;
}


struct p_worker * give_worker_inactive_groups(char * name) {
	xbt_fifo_item_t i;
	xbt_dynar_t * n;

	for(i = xbt_fifo_get_first_item(inactive_groups); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
	
		struct p_worker p_w;		
		unsigned int cpt;

		xbt_dynar_foreach(*n, cpt, p_w) {		 	
			if (!strcmp(p_w.mailbox, name)) {
				return xbt_dynar_get_ptr(*n, cpt);					
			}
		}
	}
	return NULL;
}


void updateReputation(struct p_task * t) {
	struct p_answer_worker p_a_w;
	struct p_worker * toModify;
	unsigned int cpt;

	xbt_dynar_foreach (t->w_answers, cpt, p_a_w) {
		char name[MAILBOX_SIZE];
		unsigned int cpt1;

		xbt_dynar_foreach(p_a_w.worker_names, cpt1, name) {
		// search for the worker on the workers list
			if ((toModify = give_worker_dynar(name)) == NULL) {
				if ((toModify = give_worker_inactive_groups(name)) == NULL) {
					if ((toModify = give_worker_active_groups(name)) == NULL) {
						printf("unknown worker: impossible\n");
						exit(1);
					}
				} 			
			}
			toModify->totR++;
			if (p_a_w.answer == t->final_answer) {
				toModify->totC++;
			}
			updateReputation_Sonnek (toModify);
			printf("réputation ici %d\n", toModify->reputation);	
		}
	}
}


void add_answers(struct p_task * p_t, xbt_dynar_t * w_answers, char * worker_name, unsigned long int answer) {
	unsigned int cpt;
	struct p_answer_worker w_a;
	struct p_answer_worker * toModify = NULL;
	struct p_answer_worker * toAdd = (struct p_answer_worker *) malloc(sizeof(struct p_answer_worker));

	xbt_dynar_foreach(*w_answers, cpt, w_a) {
 		if (w_a.answer == answer) {	
			toModify = xbt_dynar_get_ptr(*w_answers, cpt);
		}
	}
	if (toModify == NULL) {
		// the answer returned by the worker hasn't been returned for that request, we create it
		toAdd->answer = answer;
		toAdd->worker_names = xbt_dynar_new(sizeof(char[MAILBOX_SIZE]), NULL);
		xbt_dynar_push(toAdd->worker_names, worker_name);
		xbt_dynar_push(p_t->w_answers, toAdd);
	}
	else {
		// the answer returned by the worker has been already returned by other(s) worker(s), we just add the name of the worker to the list of workers having returned this answer
		xbt_dynar_push(toModify->worker_names, worker_name);
	}
}


void compute_majoritary_answer(struct p_task * p_t, unsigned long int * majoritary_answer, int * nb_majoritary_answer,int * nb_obtained_majoritary_answer) {
	unsigned int cpt;
	struct p_answer_worker w_a;
	int max = 0;

	xbt_dynar_foreach(p_t->w_answers, cpt, w_a) {
 		if (xbt_dynar_length(w_a.worker_names) > max) {	
			max = xbt_dynar_length(w_a.worker_names);
			*majoritary_answer = w_a.answer;
			*nb_majoritary_answer = 1;
		}
		else if (xbt_dynar_length(w_a.worker_names) == max) {
			*nb_majoritary_answer = *nb_majoritary_answer + 1;
		}
	}
	*nb_obtained_majoritary_answer = max;	
}	


void treat_answer(msg_task_t t) {
	// update the answer of the worker
	// check if all workers give the answer: if yes, look up for the majority solution
	// if all workers answer the same way, give the answer to the client and update the reputation, and put all the workers on the workers dynamic array
	// if not ask the requests to additional nodes (that the primary put in the same active group than the previous answered nodes
	struct w_task * w_t = (struct w_task *) malloc(sizeof(struct w_task));
	struct p_worker * toAddToWorkers = (struct p_worker *) malloc(sizeof(struct p_worker));
	xbt_fifo_item_t i;
	struct p_task * n;
	char process = -1;
	unsigned long int majoritary_answer = BAD_ANSWER + 1; // we initialize with a value that answer couldn't have
	int nb_majoritary_answer = 0;
	int nb_obtained_majoritary_answer = 0;

	w_t = (struct w_task *)MSG_task_get_data(t);

	// the primary have to find the element of processing_tasks that correspond to the answer
   for(i = xbt_fifo_get_first_item(processing_tasks); ((i) ? (n = (struct p_task *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		if ((strcmp(n->client, w_t->client) == 0) && (strcmp(n->task_name, w_t->task_name) == 0)) {
			printf("primary: find the task\n");
			// add the worker to w_answers
			add_answers(n, &(n->w_answers), w_t->worker_name, w_t->answer);
			n->nb_answers_received++;
			if (w_t->answer > GOOD_ANSWER) {
				n->nb_false_answers++;
			}
			else {
				n->nb_correct_answers++;
			}
			if ((n->nb_answers_received + n->nb_crashed) == n->nb_forwarded) {
				/* *******************************************************************************


				TO REPLACE BY A FUNCTION SAYING IF WE CAN AUTORISED THE ANSWER (FOR EXAMPLE AFTER 
				REPLICATION WE DON'T WAIT TO HAVE ALL WORKERS ANSWERING THE SAME ANSWER (IT IS 
				IMPOSSIBLE) SO WE NEED TO HAVE A NEW STRATEGY SAYING WHEN AN ANSWER IS CORRECT


				********************************************************************************* */
				compute_majoritary_answer(n, &majoritary_answer, &nb_majoritary_answer, &nb_obtained_majoritary_answer); 			

				printf("receive all the answers for %s %s\n", n->client, n->task_name);
				if (nb_majoritary_answer == 1) {
					// there is no ambiguity on the final result: the majority doesn't necessarly correspond to the absoulute majority

					n->final_answer = majoritary_answer;
					// send the answer to the client
					msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
					MSG_task_send(answer_client, n->client);

					// update the reputation of the workers and suppress the processing task
					updateReputation(n);
					printf("after update prints the active and workers groups\n");
					groups_print(&active_groups);
					workers_print();
					process = 1;
				}					
				else {
					// replication of the task on additional nodes and  


				}
			}
			break;
		}
	}
	printf("primary; will to remove the worker %s\n", w_t->worker_name);

	// move the worker from the active groups from the workers
	xbt_fifo_item_t j;
	xbt_dynar_t * d;

	for(j = xbt_fifo_get_first_item(active_groups); ((j) ? (d = (xbt_dynar_t *)(xbt_fifo_get_item_content(j))) : (NULL)); j = xbt_fifo_get_next_item(j)) {

		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*d, cpt, p_w) {
			printf("primary: I found the worker: cpt = %d\n", cpt);
	 		if (!strcmp(p_w.mailbox, w_t->worker_name)) {
				printf("reputation dans dynar %d\n", p_w.reputation);
				break;
			}
		}
		printf("removing: cpt=%d\n", cpt);
		xbt_dynar_remove_at(*d, cpt, toAddToWorkers);
		break;
	}
	xbt_dynar_push(workers, toAddToWorkers);
	if (process == 1) {
		// suppress the group in active groups
		if (xbt_fifo_remove(active_groups, d) != 1) {
			printf("problem detected when removing a xbt_dynar_t from active_groups\n");
		}
		else {
			printf("supression of the active group\n");
		}
		// suppress the task (because it has been accomplished) from the processing_tasks
		if(xbt_fifo_remove(processing_tasks, n) != 1) {
			printf("problem detected when removing a struct p_task from processing_tasks\n");
		}
		else {
			printf("task suppressed from the processing_tasks\n");
		}
	}

	printf("content of the inactive groups\n");
	groups_print(&inactive_groups);
	printf("content of the active groups\n");
	groups_print(&active_groups);
}


int primary (int argc, char * argv[]) {
	unsigned long int id;
	int nb_clients;
	char myMailbox[MAILBOX_SIZE];
	int nb_finalize = 0;
	msg_task_t task_todo = NULL;

	if (argc != 3) {
		exit(1);
	}

	workers = xbt_dynar_new(sizeof(struct p_worker), NULL);
	tasks = xbt_fifo_new();
	processing_tasks = xbt_fifo_new();
	active_groups = xbt_fifo_new();
	inactive_groups = xbt_fifo_new();

	id = atoi(argv[1]);
	sprintf(myMailbox, "primary-%ld", id);
	nb_clients = atoi(argv[2]);

	while (1) {
		// reception of a message
		MSG_task_receive(&(task_todo), myMailbox);

		/* the primary can receive 4 types of messages: requests from client, 
			finalization from client, join from workers, or answer to tasks of workers */
		if (!strcmp(MSG_task_get_name(task_todo), "finalize")) {
			printf("%s: I receive finalize\n", myMailbox);
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
			// the primary put the task to do in a fifo
			printf("%s: I receive a task\n", myMailbox);
			put_task_fifo(task_todo);
			MSG_task_destroy(task_todo);
			task_todo = NULL;
			tasks_print();
			try_to_treat_tasks();
			printf("end of try_to_treat_tasks\n");
			tasks_print();
			processing_tasks_print();
			printf("content of the inactive groups\n");
			groups_print(&inactive_groups);
			printf("content of the active groups\n");
			groups_print(&active_groups);
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "join", sizeof(char) * strlen("join"))) {
			// a worker want to join the system		
			printf("%s: I receive a join\n", myMailbox);	
			add_new_worker((char *)MSG_task_get_data(task_todo), myMailbox);
			workers_print();
			MSG_task_destroy(task_todo);
			task_todo = NULL;
			try_to_treat_tasks();

			printf("end of try_to_treat_tasks\n");
			tasks_print();
			processing_tasks_print();
			printf("content of the inactive groups\n");
			groups_print(&inactive_groups);
			printf("content of the active groups\n");
			groups_print(&active_groups);

			/*if (xbt_dynar_length(workers) >= NB_MIN_GROUP) {
				printf("%s: formation of inactive groups\n", myMailbox);
				formGroup_fixed_fit();
				printf("content of the inactive groups\n");
				groups_print(&inactive_groups);
				if (xbt_fifo_size(tasks) != 0) {
					treat_tasks();
					printf("after treat_tasks\n");	
					tasks_print();
				}
			}*/
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "answer", sizeof(char) * strlen("answer"))) {
			// the primary receive an answer to a request from a worker
			printf("%s: I receive an answer\n", myMailbox);
			treat_answer(task_todo);
			try_to_treat_tasks();
			MSG_task_destroy(task_todo);
			task_todo = NULL;
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "crash", sizeof(char) * strlen("crash"))) {
			printf("%s: I receive a message indicating the crash of %s\n", myMailbox, (char *) MSG_task_get_data(task_todo));
			MSG_task_destroy(task_todo);
			task_todo = NULL;
		}
		else {
			// messages incorrect
			printf("%s: I receive an incorrect message\n", myMailbox);
			MSG_task_destroy(task_todo);
			task_todo = NULL;
		}
	}

	return 0;
}


























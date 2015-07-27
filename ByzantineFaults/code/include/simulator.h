#ifndef _SIMULATOR_H
#define _SIMULATOR_H


#define FILE_NAME_SIZE 256
#define BUFFER_SIZE 256

// variables for initialised the global variable named centrality
#define CENTRALIZED 1
#define DISTRIBUTED 2
// variables for initialised the global variable named distributed_strategies
#define RANDOM 1
#define REPUTATIONS 2
// variables for initialised the global variable named simulator
#define SONNEK 1
#define ARANTES 2
// variables for initialised the global variable named group_formation_strategy
#define FIXED_FIT 1
#define FIRST_FIT 2
#define TIGHT_FIT 3
#define RANDOM_FIT 4
// variables for initialised the global variale named reputation_strategy
#define SYMMETRICAL 1
#define ASYMMETRICAL 2
#define BOINC 3
#define SONNEK_REPUTATION 4
// variables for initialised the global variable named additional_replication_strategy
#define ARANTES_REPLICATION 1
#define ITERATIVE_REDUNDANCY 2
#define PROGRESSIVE_REDUNDANCY 3
// variables for initialised the global variable named blacklist
#define BLACKLIST 1
#define NO_BLACKLIST 2
// variables for initialised the global variable named target_LOC
#define NOT_RANDOM 2


// this global variable indicate if we are using a centralised or a decentralized algorithms
int centrality;

//this global variable indicate whether we use random strategies or strategies depending on the workers' reputations to assign workers to primaries and to request the primaries with the clients' queries
int distributed_strategies;

// this global variable indicate if we use the strategies used in Sonnek, or in Arantes
int simulator;


// this global variable indicate the strategy used to form strategy to group workers togather. A primary do those strategies among the workers it handles to form groups on which we will replicate tasks
int group_formation_strategy;
// value used to indicate the number of workers wanted in case of a fixed-fit strategy
int group_formation_fixed_number;

// value used to know the minimum number of workers we want in groups of workers in case of first-fit, random-fit and tight-fit strategies
int group_formation_min_number;
// value used to know the maximum number of workers we want in groups of workers in case of first-fit, random-fit and tight-fit strategies
int group_formation_max_number;


// this global variable indicate the strategy used to update the reputation
int reputation_strategy;
// value used to increase the reputation in case of symmetrical or asymmetrical strategy
double reputation_x;
// value used to decrease the reputation in case of asymmetrical strategy
double reputation_y;


// this global variable indicate the strategy used to do additional replication on nodes
int additional_replication_strategy;
// value used to indicate the difference number between supposed good and supposed bad answers
int additional_replication_value_difference;

// this global varibale indicates the maximum number of workers we want in the system
int nb_workers;
// this global variable indicates the number of clients in the system
int nb_clients;
// this global variable indicates the maximum number of primaries we want in the system
int nb_primaries;

// this global variable permits to know the starting time when the nodes we are using where trace
double time_start;

// this gloval variable serves to execute the load_balancing function
int number_workers_too_high;

// this global variable serves to know if we want to do some blacklist or not
int blacklist;

//this global variable serves to indicate if we would like to use a RANDOM target_LOC for clients during the stationary regime
int radom_target_LOC;
// this global variable serves to indicate the value of target_LOC wanted by clients when the application is in a transitional regime
double value_target_LOC_start;
// this global variable serves to indicate the value of target_LOC wanted by clients when the application is in a stationary regime
double value_target_LOC_stationary;

// this global variable serves to indicate the duration of the tasks sent by clients
int task_compute_duration;
// this global variable serves to indicate the size of the message
int task_message_size;

// this global variable serves to indicate if we have achieved the stationary regime
int stationary_regime;



void print_workers_presence(void);

void fill_workers_presence_array(char * file_database);

#endif

#ifndef _SIMULATOR_H
#define _SIMULATOR_H


#define FILE_NAME_SIZE 256

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

// this global varibale indicate the maximum number of workers we want in the system
int nb_max_workers;


// this global variable permits to know the starting time when the nodes we are using where trace
double time_start;

#endif

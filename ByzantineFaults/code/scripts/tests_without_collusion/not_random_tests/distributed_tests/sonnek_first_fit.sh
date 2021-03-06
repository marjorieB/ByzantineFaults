#!/bin/bash

cd ~/distributed_project/bin

nb_workers=1500

nb_primaries=20

nb_too_high=150

declare -a clients=(1000 2500 5000 7500 10000)

# test for the first_fit symmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST SONNEK SYMMETRICAL 0.10 FIRST_FIT 3 13 NOT_RANDOM 0.90
done

# test for the first_fit symmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST SONNEK SYMMETRICAL 0.10 FIRST_FIT 3 13 NOT_RANDOM 0.90
done

# test for the first_fit asymmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST SONNEK ASYMMETRICAL 0.10 0.20 FIRST_FIT 3 13 NOT_RANDOM 0.90
done

# test for the first_fit asymmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST SONNEK ASYMMETRICAL 0.10 0.20 FIRST_FIT 3 13 NOT_RANDOM 0.90
done


# test for the first_fit boinc
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST SONNEK BOINC FIRST_FIT 3 13 NOT_RANDOM 0.90
done

# test for the first_fit boinc
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST SONNEK BOINC FIRST_FIT 3 13 NOT_RANDOM 0.90
done


# test for the first_fit sonnek_reputation
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST SONNEK SONNEK_REPUTATION FIRST_FIT 3 13 NOT_RANDOM 0.90
done

# test for the first_fit sonnek_reputation
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST SONNEK SONNEK_REPUTATION FIRST_FIT 3 13 NOT_RANDOM 0.90
done



























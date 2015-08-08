#!/bin/bash

cd ~/distributed_project/bin

nb_workers=1500

declare -a clients=(1000 2500 5000 7500 10000)

# test for the first_fit symmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK SYMMETRICAL 0.10 FIRST_FIT 3 13 RANDOM
done

# test for the first_fit asymmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK ASYMMETRICAL 0.10 0.20 FIRST_FIT 3 13 RANDOM
done


# test for the first_fit boinc
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK BOINC FIRST_FIT 3 13 RANDOM
done


# test for the first_fit sonnek_reputation
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK SONNEK_REPUTATION FIRST_FIT 3 13 RANDOM
done


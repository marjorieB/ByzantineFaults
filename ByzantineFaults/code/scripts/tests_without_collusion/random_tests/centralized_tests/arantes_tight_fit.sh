#!/bin/bash

cd ~/distributed_project/bin

nb_workers=1500

declare -a clients=(1000 2500 5000 7500 10000)


# test for the tight_fit symmetrical
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED ARANTES SYMMETRICAL 0.10 TIGHT_FIT 2 13 RANDOM
done


# test for the tight_fit asymmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED ARANTES ASYMMETRICAL 0.10 0.20 TIGHT_FIT 2 13 RANDOM
done


# test for the tight_fit boinc
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED ARANTES BOINC TIGHT_FIT 2 13 RANDOM
done


# test for the tight_fit sonnek_reputation

for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED ARANTES SONNEK_REPUTATION TIGHT_FIT 2 13 RANDOM
done




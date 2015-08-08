#!/bin/bash

cd ../../bin


#TEST FOR SONNEK RANDOM_FIT NOT_RANDOM_TARGET_LOC

nb_workers=1500

# test for the random_fit symmetrical

for nb_clients in 1000 2000 4000 6000 8000 10000
do
	./my_boinc ../database_file/boinc_file $nb_workers $nb_clients 13 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK SYMMETRICAL 0.10 RANDOM_FIT 3 13 NOT_RANDOM 0.95
done


# test for the random_fit asymmetrical

for nb_clients in 1000 2000 4000 6000 8000 10000
do
	./my_boinc ../database_file/boinc_file $nb_workers $nb_clients 13 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK ASYMMETRICAL 0.10 0.20 RANDOM_FIT 3 13 NOT_RANDOM 0.95
done


# test for the random_fit boinc
for nb_clients in 1000 2000 4000 6000 8000 10000
do
	./my_boinc ../database_file/boinc_file $nb_workers $nb_clients 13 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK BOINC RANDOM_FIT 3 13 NOT_RANDOM 0.95
done



# test for the random_fit sonnek_reputation

for nb_clients in 1000 2000 4000 6000 8000 10000
do
	./my_boinc ../database_file/boinc_file $nb_workers $nb_clients 13 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK SONNEK_REPUTATION RANDOM_FIT 3 13 NOT_RANDOM 0.95
done



#TEST FOR SONNEK RANDOM_TARGET_LOC

# test for the random_fit symmetrical

for nb_clients in 1000 2000 4000 6000 8000 10000
do
	./my_boinc ../database_file/boinc_file $nb_workers $nb_clients 13 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK SYMMETRICAL 0.10 RANDOM_FIT 3 13 RANDOM
done


# test for the random_fit asymmetrical

for nb_clients in 1000 2000 4000 6000 8000 10000
do
	./my_boinc ../database_file/boinc_file $nb_workers $nb_clients 13 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK ASYMMETRICAL 0.10 0.20 RANDOM_FIT 3 13 RANDOM
done



# test for the random_fit boinc

for nb_clients in 1000 2000 4000 6000 8000 10000
do
	./my_boinc ../database_file/boinc_file $nb_workers $nb_clients 13 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK BOINC RANDOM_FIT 3 13 RANDOM
done


# test for the random_fit sonnek_reputation
for nb_clients in 1000 2000 4000 6000 8000 10000
do
	./my_boinc ../database_file/boinc_file $nb_workers $nb_clients 13 ../platforms/deployement_centralized_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_centralized_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 CENTRALIZED SONNEK SONNEK_REPUTATION RANDOM_FIT 3 13 RANDOM
done

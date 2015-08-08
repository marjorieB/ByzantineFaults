#!/bin/bash

cd ~/distributed_project/bin

nb_workers=1500

nb_primaries=20

nb_too_high=150

declare -a clients=(1000 2500 5000 7500 10000)

# test for the tight_fit symmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST SONNEK SYMMETRICAL 0.10 TIGHT_FIT 3 13 RANDOM
done

# test for the tight_fit symmetrical
for nb_clients in ${clients[*]}
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST SONNEK SYMMETRICAL 0.10 TIGHT_FIT 3 13 RANDOM
done


# test for the tight_fit asymmetrical
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST SONNEK ASYMMETRICAL 0.10 0.20 TIGHT_FIT 3 13 RANDOM
done

# test for the tight_fit asymmetrical
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST SONNEK ASYMMETRICAL 0.10 0.20 TIGHT_FIT 3 13 RANDOM
done

# test for the tight_fit boinc
for nb_clients in ${clients[*]}		
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST SONNEK BOINC TIGHT_FIT 3 13 RANDOM
done

# test for the tight_fit boinc
for nb_clients in ${clients[*]}		
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST SONNEK BOINC TIGHT_FIT 3 13 RANDOM
done


# test for the tight_fit sonnek_reputation
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST SONNEK SONNEK_REPUTATION TIGHT_FIT 3 13 RANDOM
done

# test for the tight_fit sonnek_reputation
for nb_clients in ${clients[*]}	
do
	./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST SONNEK SONNEK_REPUTATION TIGHT_FIT 3 13 RANDOM
done




























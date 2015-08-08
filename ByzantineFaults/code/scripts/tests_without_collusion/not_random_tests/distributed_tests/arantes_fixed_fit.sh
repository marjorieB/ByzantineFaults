#!/bin/bash

cd ~/distributed_project/bin

nb_workers=1500

nb_primaries=20

nb_too_high=150

declare -a clients=(1000 2500 5000 7500 10000)


#TEST FOR ARANTES FIXED_FIT WITH ITERATIVE_REDUNDANCY

# test for the fixed_fit symmetrical
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST ARANTES SYMMETRICAL 0.10 FIXED_FIT $k ITERATIVE_REDUNDANCY `expr $k / 2 + 1`
	done
done

#TEST FOR ARANTES FIXED_FIT WITH ITERATIVE_REDUNDANCY

# test for the fixed_fit symmetrical
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST ARANTES SYMMETRICAL 0.10 FIXED_FIT $k ITERATIVE_REDUNDANCY `expr $k / 2 + 1`
	done
done


# test for the fixed_fit asymmetrical
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST ARANTES ASYMMETRICAL 0.10 0.20 FIXED_FIT $k ITERATIVE_REDUNDANCY `expr $k / 2 + 1`
	done
done

# test for the fixed_fit asymmetrical
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST ARANTES ASYMMETRICAL 0.10 0.20 FIXED_FIT $k ITERATIVE_REDUNDANCY `expr $k / 2 + 1`
	done
done

# test for the fixed_fit boinc
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST ARANTES BOINC FIXED_FIT $k ITERATIVE_REDUNDANCY `expr $k / 2 + 1`
	done
done

# test for the fixed_fit boinc
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST ARANTES BOINC FIXED_FIT $k ITERATIVE_REDUNDANCY `expr $k / 2 + 1`
	done
done


# test for the fixed_fit sonnek_reputation
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST ARANTES SONNEK_REPUTATION FIXED_FIT $k ITERATIVE_REDUNDANCY `expr $k / 2 + 1`
	done
done

# test for the fixed_fit sonnek_reputation
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST ARANTES SONNEK_REPUTATION FIXED_FIT $k ITERATIVE_REDUNDANCY `expr $k / 2 + 1`
	done
done

#TEST FOR ARANTES FIXED_FIT WITH PROGRESSIVE_REDUNDANCY

# test for the fixed_fit symmetrical
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST ARANTES SYMMETRICAL 0.10 FIXED_FIT $k PROGRESSIVE_REDUNDANCY
	done
done

# test for the fixed_fit symmetrical
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST ARANTES SYMMETRICAL 0.10 FIXED_FIT $k PROGRESSIVE_REDUNDANCY
	done
done

# test for the fixed_fit asymmetrical
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST ARANTES ASYMMETRICAL 0.10 0.20 FIXED_FIT $k PROGRESSIVE_REDUNDANCY
	done
done

# test for the fixed_fit asymmetrical
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST ARANTES ASYMMETRICAL 0.10 0.20 FIXED_FIT $k PROGRESSIVE_REDUNDANCY
	done
done

# test for the fixed_fit boinc
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST ARANTES BOINC FIXED_FIT $k PROGRESSIVE_REDUNDANCY
	done
done


# test for the fixed_fit boinc
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST ARANTES BOINC FIXED_FIT $k PROGRESSIVE_REDUNDANCY
	done
done


# test for the fixed_fit sonnek_reputation
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED RANDOM $nb_primaries $nb_too_high NO_BLACKLIST ARANTES SONNEK_REPUTATION FIXED_FIT $k PROGRESSIVE_REDUNDANCY
	done
done

# test for the fixed_fit sonnek_reputation
for nb_clients in ${clients[*]}
do
	for k in 3 7 13
	do
		./my_boinc $nb_workers $nb_clients 20 ../platforms/deployement_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml ../platforms/platform_distributed_$nb_primaries\primaries_$nb_workers\workers_$nb_clients\clients.xml 21600000000000 1000000000 DISTRIBUTED REPUTATIONS $nb_primaries $nb_too_high NO_BLACKLIST ARANTES SONNEK_REPUTATION FIXED_FIT $k PROGRESSIVE_REDUNDANCY
	done
done
















































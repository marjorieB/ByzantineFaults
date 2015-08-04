#!/bin/bash


for nb_clients in 2000 4000 6000 8000 10000	
do
	echo "create_xml_platform(conn, 1500, $nb_clients, 1, \"/home/marjo/stage/logiciel/Simgrid_et_co/distributed_project/platforms\");"
	echo ""
done

for nb_clients in 2000 4000 6000 8000 10000	
do
	echo "create_xml_deployement(conn, 1500, $nb_clients, 1, \"/home/marjo/stage/logiciel/Simgrid_et_co/distributed_project/platforms\");"
	echo ""
done



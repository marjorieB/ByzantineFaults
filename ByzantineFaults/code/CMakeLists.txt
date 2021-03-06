CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(MY_BOINC)
SET(CMAKE_BUILD_TYPE Debug)

SET(SIMGRID_LIB /home/marjo/stage/logiciel/Simgrid_et_co/simgrid/lib CACHE PATH "SimGrid library")
SET(MY_INCLUDE /home/marjo/stage/logiciel/Simgrid_et_co/distributed_project/include CACHE PATH "my_project include")
SET(MYSQL_LIB /usr/lib/ CACHE PATH "mysql_lib")
SET(INCLUDE_MYSQL /usr/include/ CACHE PATH "mysql_include")
SET(SIMGRID_INCLUDE /home/marjo/stage/logiciel/Simgrid_et_co/simgrid/include CACHE PATH "SimGrid include")

OPTION(WARNINGS_ARE_ERRORS "Warning become errors during compilation." ON)

LINK_DIRECTORIES(${SIMGRID_LIB} ${MYSQL_LIB})
INCLUDE_DIRECTORIES(${SIMGRID_INCLUDE} ${MY_INCLUDE} ${INCLUDE_MYSQL})

SET(MY_BOINC_SRC
	src/simulator.c
	src/reputation_strategy.c
	src/group_formation_strategy.c
	src/additional_replication_strategy.c
	src/primary.c
	src/worker.c
	src/client.c
	src/first_primary.c
)

ADD_EXECUTABLE(my_boinc ${MY_BOINC_SRC})
TARGET_LINK_LIBRARIES(my_boinc mysqlclient)
TARGET_LINK_LIBRARIES(my_boinc m) #to include the math library
TARGET_LINK_LIBRARIES(my_boinc simgrid) #to include simgrid

#INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/my_boinc
        #DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

IF(WARNINGS_ARE_ERRORS)
  set(warnCFLAGS "-Wall -Wunused -Wmissing-prototypes -pg -Wmissing-declarations -Wpointer-arith -Wchar-subscripts -Wcomment -Wformat -Wwrite-strings -Wno-clobbered -Wno-unused-function -Wno-unused-parameter -Wno-strict-aliasing -Wno-format-nonliteral -Werror ")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warnCFLAGS}")
ENDIF(WARNINGS_ARE_ERRORS)


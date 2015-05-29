CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(MY_BOINC)
SET(CMAKE_BUILD_TYPE Debug)

SET(SIMGRID_LIB /home/marjo/stage/logiciel/Simgrid_et_co/simgrid/lib CACHE PATH "SimGrid library")
SET(MY_INCLUDE /home/marjo/stage/logiciel/Simgrid_et_co/projet/include CACHE PATH "my_project include")
SET(SIMGRID_INCLUDE /home/marjo/stage/logiciel/Simgrid_et_co/simgrid/include CACHE PATH "SimGrid include")
OPTION(WARNINGS_ARE_ERRORS "Warning become errors during compilation." ON)

LINK_DIRECTORIES(${SIMGRID_LIB})
INCLUDE_DIRECTORIES(${SIMGRID_INCLUDE} ${MY_INCLUDE})

SET(MY_BOINC_SRC
	src/sonnek_simulator.c
	src/reputation_strategy.c
	src/group_formation_strategy.c
	src/primary.c
	src/worker.c
	src/client.c
)

ADD_EXECUTABLE(my_boinc ${MY_BOINC_SRC})
TARGET_LINK_LIBRARIES(my_boinc m)
TARGET_LINK_LIBRARIES(my_boinc simgrid)

#INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/my_boinc
        #DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

IF(WARNINGS_ARE_ERRORS)
  set(warnCFLAGS "-Wall -Wunused -Wmissing-prototypes -Wmissing-declarations -Wpointer-arith -Wchar-subscripts -Wcomment -Wformat -Wwrite-strings -Wno-clobbered -Wno-unused-function -Wno-unused-parameter -Wno-strict-aliasing -Wno-format-nonliteral -Werror ")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warnCFLAGS}")
ENDIF(WARNINGS_ARE_ERRORS)


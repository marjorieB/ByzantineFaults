# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/marjo/stage/logiciel/Simgrid_et_co/projet

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/marjo/stage/logiciel/Simgrid_et_co/projet

# Include any dependencies generated for this target.
include CMakeFiles/bin/myBoinc.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/bin/myBoinc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/bin/myBoinc.dir/flags.make

CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o: CMakeFiles/bin/myBoinc.dir/flags.make
CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o: src/reputation_strategy.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/marjo/stage/logiciel/Simgrid_et_co/projet/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o   -c /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/reputation_strategy.c

CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/reputation_strategy.c > CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.i

CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/reputation_strategy.c -o CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.s

CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o.requires:
.PHONY : CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o.requires

CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o.provides: CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o.requires
	$(MAKE) -f CMakeFiles/bin/myBoinc.dir/build.make CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o.provides.build
.PHONY : CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o.provides

CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o.provides.build: CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o

CMakeFiles/bin/myBoinc.dir/src/workers.c.o: CMakeFiles/bin/myBoinc.dir/flags.make
CMakeFiles/bin/myBoinc.dir/src/workers.c.o: src/workers.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/marjo/stage/logiciel/Simgrid_et_co/projet/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/bin/myBoinc.dir/src/workers.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bin/myBoinc.dir/src/workers.c.o   -c /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/workers.c

CMakeFiles/bin/myBoinc.dir/src/workers.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bin/myBoinc.dir/src/workers.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/workers.c > CMakeFiles/bin/myBoinc.dir/src/workers.c.i

CMakeFiles/bin/myBoinc.dir/src/workers.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bin/myBoinc.dir/src/workers.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/workers.c -o CMakeFiles/bin/myBoinc.dir/src/workers.c.s

CMakeFiles/bin/myBoinc.dir/src/workers.c.o.requires:
.PHONY : CMakeFiles/bin/myBoinc.dir/src/workers.c.o.requires

CMakeFiles/bin/myBoinc.dir/src/workers.c.o.provides: CMakeFiles/bin/myBoinc.dir/src/workers.c.o.requires
	$(MAKE) -f CMakeFiles/bin/myBoinc.dir/build.make CMakeFiles/bin/myBoinc.dir/src/workers.c.o.provides.build
.PHONY : CMakeFiles/bin/myBoinc.dir/src/workers.c.o.provides

CMakeFiles/bin/myBoinc.dir/src/workers.c.o.provides.build: CMakeFiles/bin/myBoinc.dir/src/workers.c.o

CMakeFiles/bin/myBoinc.dir/src/client.c.o: CMakeFiles/bin/myBoinc.dir/flags.make
CMakeFiles/bin/myBoinc.dir/src/client.c.o: src/client.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/marjo/stage/logiciel/Simgrid_et_co/projet/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/bin/myBoinc.dir/src/client.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bin/myBoinc.dir/src/client.c.o   -c /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/client.c

CMakeFiles/bin/myBoinc.dir/src/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bin/myBoinc.dir/src/client.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/client.c > CMakeFiles/bin/myBoinc.dir/src/client.c.i

CMakeFiles/bin/myBoinc.dir/src/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bin/myBoinc.dir/src/client.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/client.c -o CMakeFiles/bin/myBoinc.dir/src/client.c.s

CMakeFiles/bin/myBoinc.dir/src/client.c.o.requires:
.PHONY : CMakeFiles/bin/myBoinc.dir/src/client.c.o.requires

CMakeFiles/bin/myBoinc.dir/src/client.c.o.provides: CMakeFiles/bin/myBoinc.dir/src/client.c.o.requires
	$(MAKE) -f CMakeFiles/bin/myBoinc.dir/build.make CMakeFiles/bin/myBoinc.dir/src/client.c.o.provides.build
.PHONY : CMakeFiles/bin/myBoinc.dir/src/client.c.o.provides

CMakeFiles/bin/myBoinc.dir/src/client.c.o.provides.build: CMakeFiles/bin/myBoinc.dir/src/client.c.o

CMakeFiles/bin/myBoinc.dir/src/primary.c.o: CMakeFiles/bin/myBoinc.dir/flags.make
CMakeFiles/bin/myBoinc.dir/src/primary.c.o: src/primary.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/marjo/stage/logiciel/Simgrid_et_co/projet/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/bin/myBoinc.dir/src/primary.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bin/myBoinc.dir/src/primary.c.o   -c /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/primary.c

CMakeFiles/bin/myBoinc.dir/src/primary.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bin/myBoinc.dir/src/primary.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/primary.c > CMakeFiles/bin/myBoinc.dir/src/primary.c.i

CMakeFiles/bin/myBoinc.dir/src/primary.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bin/myBoinc.dir/src/primary.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/primary.c -o CMakeFiles/bin/myBoinc.dir/src/primary.c.s

CMakeFiles/bin/myBoinc.dir/src/primary.c.o.requires:
.PHONY : CMakeFiles/bin/myBoinc.dir/src/primary.c.o.requires

CMakeFiles/bin/myBoinc.dir/src/primary.c.o.provides: CMakeFiles/bin/myBoinc.dir/src/primary.c.o.requires
	$(MAKE) -f CMakeFiles/bin/myBoinc.dir/build.make CMakeFiles/bin/myBoinc.dir/src/primary.c.o.provides.build
.PHONY : CMakeFiles/bin/myBoinc.dir/src/primary.c.o.provides

CMakeFiles/bin/myBoinc.dir/src/primary.c.o.provides.build: CMakeFiles/bin/myBoinc.dir/src/primary.c.o

CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o: CMakeFiles/bin/myBoinc.dir/flags.make
CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o: src/group_formation_strategy.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/marjo/stage/logiciel/Simgrid_et_co/projet/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o   -c /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/group_formation_strategy.c

CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/group_formation_strategy.c > CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.i

CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/group_formation_strategy.c -o CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.s

CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o.requires:
.PHONY : CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o.requires

CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o.provides: CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o.requires
	$(MAKE) -f CMakeFiles/bin/myBoinc.dir/build.make CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o.provides.build
.PHONY : CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o.provides

CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o.provides.build: CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o

CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o: CMakeFiles/bin/myBoinc.dir/flags.make
CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o: src/Sonnek_simulator.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/marjo/stage/logiciel/Simgrid_et_co/projet/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o   -c /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/Sonnek_simulator.c

CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/Sonnek_simulator.c > CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.i

CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/marjo/stage/logiciel/Simgrid_et_co/projet/src/Sonnek_simulator.c -o CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.s

CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o.requires:
.PHONY : CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o.requires

CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o.provides: CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o.requires
	$(MAKE) -f CMakeFiles/bin/myBoinc.dir/build.make CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o.provides.build
.PHONY : CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o.provides

CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o.provides.build: CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o

# Object files for target bin/myBoinc
bin/myBoinc_OBJECTS = \
"CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o" \
"CMakeFiles/bin/myBoinc.dir/src/workers.c.o" \
"CMakeFiles/bin/myBoinc.dir/src/client.c.o" \
"CMakeFiles/bin/myBoinc.dir/src/primary.c.o" \
"CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o" \
"CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o"

# External object files for target bin/myBoinc
bin/myBoinc_EXTERNAL_OBJECTS =

bin/myBoinc: CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o
bin/myBoinc: CMakeFiles/bin/myBoinc.dir/src/workers.c.o
bin/myBoinc: CMakeFiles/bin/myBoinc.dir/src/client.c.o
bin/myBoinc: CMakeFiles/bin/myBoinc.dir/src/primary.c.o
bin/myBoinc: CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o
bin/myBoinc: CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o
bin/myBoinc: CMakeFiles/bin/myBoinc.dir/build.make
bin/myBoinc: CMakeFiles/bin/myBoinc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable bin/myBoinc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bin/myBoinc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/bin/myBoinc.dir/build: bin/myBoinc
.PHONY : CMakeFiles/bin/myBoinc.dir/build

CMakeFiles/bin/myBoinc.dir/requires: CMakeFiles/bin/myBoinc.dir/src/reputation_strategy.c.o.requires
CMakeFiles/bin/myBoinc.dir/requires: CMakeFiles/bin/myBoinc.dir/src/workers.c.o.requires
CMakeFiles/bin/myBoinc.dir/requires: CMakeFiles/bin/myBoinc.dir/src/client.c.o.requires
CMakeFiles/bin/myBoinc.dir/requires: CMakeFiles/bin/myBoinc.dir/src/primary.c.o.requires
CMakeFiles/bin/myBoinc.dir/requires: CMakeFiles/bin/myBoinc.dir/src/group_formation_strategy.c.o.requires
CMakeFiles/bin/myBoinc.dir/requires: CMakeFiles/bin/myBoinc.dir/src/Sonnek_simulator.c.o.requires
.PHONY : CMakeFiles/bin/myBoinc.dir/requires

CMakeFiles/bin/myBoinc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/bin/myBoinc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/bin/myBoinc.dir/clean

CMakeFiles/bin/myBoinc.dir/depend:
	cd /home/marjo/stage/logiciel/Simgrid_et_co/projet && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/marjo/stage/logiciel/Simgrid_et_co/projet /home/marjo/stage/logiciel/Simgrid_et_co/projet /home/marjo/stage/logiciel/Simgrid_et_co/projet /home/marjo/stage/logiciel/Simgrid_et_co/projet /home/marjo/stage/logiciel/Simgrid_et_co/projet/CMakeFiles/bin/myBoinc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bin/myBoinc.dir/depend

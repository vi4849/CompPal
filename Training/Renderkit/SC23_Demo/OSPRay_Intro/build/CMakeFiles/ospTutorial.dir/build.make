# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/build

# Include any dependencies generated for this target.
include CMakeFiles/ospTutorial.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ospTutorial.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ospTutorial.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ospTutorial.dir/flags.make

CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o: CMakeFiles/ospTutorial.dir/flags.make
CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o: ../src/ospTutorial.cpp
CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o: CMakeFiles/ospTutorial.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o -MF CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o.d -o CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o -c /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/src/ospTutorial.cpp

CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/src/ospTutorial.cpp > CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.i

CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/src/ospTutorial.cpp -o CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.s

# Object files for target ospTutorial
ospTutorial_OBJECTS = \
"CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o"

# External object files for target ospTutorial
ospTutorial_EXTERNAL_OBJECTS =

../bin/ospTutorial: CMakeFiles/ospTutorial.dir/src/ospTutorial.cpp.o
../bin/ospTutorial: CMakeFiles/ospTutorial.dir/build.make
../bin/ospTutorial: CMakeFiles/ospTutorial.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/ospTutorial"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ospTutorial.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ospTutorial.dir/build: ../bin/ospTutorial
.PHONY : CMakeFiles/ospTutorial.dir/build

CMakeFiles/ospTutorial.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ospTutorial.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ospTutorial.dir/clean

CMakeFiles/ospTutorial.dir/depend:
	cd /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/build /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/build /home/ue67fcfb20b4827a1ed6842c10ec58c1/Training/SC23_Demo/OSPRay_Intro/build/CMakeFiles/ospTutorial.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ospTutorial.dir/depend

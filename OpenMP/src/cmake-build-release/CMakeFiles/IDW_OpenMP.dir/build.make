# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_SOURCE_DIR = /tmp/tmp.DDgOhPIOFp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.DDgOhPIOFp/cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/IDW_OpenMP.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/IDW_OpenMP.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/IDW_OpenMP.dir/flags.make

CMakeFiles/IDW_OpenMP.dir/main.cpp.o: CMakeFiles/IDW_OpenMP.dir/flags.make
CMakeFiles/IDW_OpenMP.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.DDgOhPIOFp/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/IDW_OpenMP.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/IDW_OpenMP.dir/main.cpp.o -c /tmp/tmp.DDgOhPIOFp/main.cpp

CMakeFiles/IDW_OpenMP.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDW_OpenMP.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.DDgOhPIOFp/main.cpp > CMakeFiles/IDW_OpenMP.dir/main.cpp.i

CMakeFiles/IDW_OpenMP.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDW_OpenMP.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.DDgOhPIOFp/main.cpp -o CMakeFiles/IDW_OpenMP.dir/main.cpp.s

CMakeFiles/IDW_OpenMP.dir/Raster.cpp.o: CMakeFiles/IDW_OpenMP.dir/flags.make
CMakeFiles/IDW_OpenMP.dir/Raster.cpp.o: ../Raster.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.DDgOhPIOFp/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/IDW_OpenMP.dir/Raster.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/IDW_OpenMP.dir/Raster.cpp.o -c /tmp/tmp.DDgOhPIOFp/Raster.cpp

CMakeFiles/IDW_OpenMP.dir/Raster.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDW_OpenMP.dir/Raster.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.DDgOhPIOFp/Raster.cpp > CMakeFiles/IDW_OpenMP.dir/Raster.cpp.i

CMakeFiles/IDW_OpenMP.dir/Raster.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDW_OpenMP.dir/Raster.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.DDgOhPIOFp/Raster.cpp -o CMakeFiles/IDW_OpenMP.dir/Raster.cpp.s

CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.o: CMakeFiles/IDW_OpenMP.dir/flags.make
CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.o: ../Metodos.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.DDgOhPIOFp/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.o -c /tmp/tmp.DDgOhPIOFp/Metodos.cpp

CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.DDgOhPIOFp/Metodos.cpp > CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.i

CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.DDgOhPIOFp/Metodos.cpp -o CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.s

# Object files for target IDW_OpenMP
IDW_OpenMP_OBJECTS = \
"CMakeFiles/IDW_OpenMP.dir/main.cpp.o" \
"CMakeFiles/IDW_OpenMP.dir/Raster.cpp.o" \
"CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.o"

# External object files for target IDW_OpenMP
IDW_OpenMP_EXTERNAL_OBJECTS =

IDW_OpenMP: CMakeFiles/IDW_OpenMP.dir/main.cpp.o
IDW_OpenMP: CMakeFiles/IDW_OpenMP.dir/Raster.cpp.o
IDW_OpenMP: CMakeFiles/IDW_OpenMP.dir/Metodos.cpp.o
IDW_OpenMP: CMakeFiles/IDW_OpenMP.dir/build.make
IDW_OpenMP: CMakeFiles/IDW_OpenMP.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.DDgOhPIOFp/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable IDW_OpenMP"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/IDW_OpenMP.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/IDW_OpenMP.dir/build: IDW_OpenMP
.PHONY : CMakeFiles/IDW_OpenMP.dir/build

CMakeFiles/IDW_OpenMP.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/IDW_OpenMP.dir/cmake_clean.cmake
.PHONY : CMakeFiles/IDW_OpenMP.dir/clean

CMakeFiles/IDW_OpenMP.dir/depend:
	cd /tmp/tmp.DDgOhPIOFp/cmake-build-release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.DDgOhPIOFp /tmp/tmp.DDgOhPIOFp /tmp/tmp.DDgOhPIOFp/cmake-build-release /tmp/tmp.DDgOhPIOFp/cmake-build-release /tmp/tmp.DDgOhPIOFp/cmake-build-release/CMakeFiles/IDW_OpenMP.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/IDW_OpenMP.dir/depend


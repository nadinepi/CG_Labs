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
CMAKE_SOURCE_DIR = /home/leviwfrey/LUND/EDAF80/CG_Labs

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/leviwfrey/LUND/EDAF80/CG_Labs

# Include any dependencies generated for this target.
include src/EDAF80/CMakeFiles/parametric_shapes.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/EDAF80/CMakeFiles/parametric_shapes.dir/compiler_depend.make

# Include the progress variables for this target.
include src/EDAF80/CMakeFiles/parametric_shapes.dir/progress.make

# Include the compile flags for this target's objects.
include src/EDAF80/CMakeFiles/parametric_shapes.dir/flags.make

src/EDAF80/CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o: src/EDAF80/CMakeFiles/parametric_shapes.dir/flags.make
src/EDAF80/CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o: src/EDAF80/parametric_shapes.cpp
src/EDAF80/CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o: src/EDAF80/CMakeFiles/parametric_shapes.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/leviwfrey/LUND/EDAF80/CG_Labs/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/EDAF80/CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o"
	cd /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/EDAF80/CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o -MF CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o.d -o CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o -c /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80/parametric_shapes.cpp

src/EDAF80/CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.i"
	cd /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80/parametric_shapes.cpp > CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.i

src/EDAF80/CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.s"
	cd /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80/parametric_shapes.cpp -o CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.s

# Object files for target parametric_shapes
parametric_shapes_OBJECTS = \
"CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o"

# External object files for target parametric_shapes
parametric_shapes_EXTERNAL_OBJECTS =

src/EDAF80/libparametric_shapes.a: src/EDAF80/CMakeFiles/parametric_shapes.dir/parametric_shapes.cpp.o
src/EDAF80/libparametric_shapes.a: src/EDAF80/CMakeFiles/parametric_shapes.dir/build.make
src/EDAF80/libparametric_shapes.a: src/EDAF80/CMakeFiles/parametric_shapes.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/leviwfrey/LUND/EDAF80/CG_Labs/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libparametric_shapes.a"
	cd /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80 && $(CMAKE_COMMAND) -P CMakeFiles/parametric_shapes.dir/cmake_clean_target.cmake
	cd /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/parametric_shapes.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/EDAF80/CMakeFiles/parametric_shapes.dir/build: src/EDAF80/libparametric_shapes.a
.PHONY : src/EDAF80/CMakeFiles/parametric_shapes.dir/build

src/EDAF80/CMakeFiles/parametric_shapes.dir/clean:
	cd /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80 && $(CMAKE_COMMAND) -P CMakeFiles/parametric_shapes.dir/cmake_clean.cmake
.PHONY : src/EDAF80/CMakeFiles/parametric_shapes.dir/clean

src/EDAF80/CMakeFiles/parametric_shapes.dir/depend:
	cd /home/leviwfrey/LUND/EDAF80/CG_Labs && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/leviwfrey/LUND/EDAF80/CG_Labs /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80 /home/leviwfrey/LUND/EDAF80/CG_Labs /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80 /home/leviwfrey/LUND/EDAF80/CG_Labs/src/EDAF80/CMakeFiles/parametric_shapes.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/EDAF80/CMakeFiles/parametric_shapes.dir/depend

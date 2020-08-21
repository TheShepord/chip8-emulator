# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /c/Users/lucas/Documents/Code/2020/chip8-emulator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /c/Users/lucas/Documents/Code/2020/chip8-emulator

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /c/Users/lucas/Documents/Code/2020/chip8-emulator/CMakeFiles /c/Users/lucas/Documents/Code/2020/chip8-emulator/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /c/Users/lucas/Documents/Code/2020/chip8-emulator/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named Chip8_Emulator

# Build rule for target.
Chip8_Emulator: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 Chip8_Emulator
.PHONY : Chip8_Emulator

# fast build rule for target.
Chip8_Emulator/fast:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/build
.PHONY : Chip8_Emulator/fast

src/chip8.o: src/chip8.cpp.o

.PHONY : src/chip8.o

# target to build an object file
src/chip8.cpp.o:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/chip8.cpp.o
.PHONY : src/chip8.cpp.o

src/chip8.i: src/chip8.cpp.i

.PHONY : src/chip8.i

# target to preprocess a source file
src/chip8.cpp.i:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/chip8.cpp.i
.PHONY : src/chip8.cpp.i

src/chip8.s: src/chip8.cpp.s

.PHONY : src/chip8.s

# target to generate assembly for a file
src/chip8.cpp.s:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/chip8.cpp.s
.PHONY : src/chip8.cpp.s

src/clock.o: src/clock.cpp.o

.PHONY : src/clock.o

# target to build an object file
src/clock.cpp.o:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/clock.cpp.o
.PHONY : src/clock.cpp.o

src/clock.i: src/clock.cpp.i

.PHONY : src/clock.i

# target to preprocess a source file
src/clock.cpp.i:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/clock.cpp.i
.PHONY : src/clock.cpp.i

src/clock.s: src/clock.cpp.s

.PHONY : src/clock.s

# target to generate assembly for a file
src/clock.cpp.s:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/clock.cpp.s
.PHONY : src/clock.cpp.s

src/main.o: src/main.cpp.o

.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i

.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s

.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) -f CMakeFiles/Chip8_Emulator.dir/build.make CMakeFiles/Chip8_Emulator.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... Chip8_Emulator"
	@echo "... src/chip8.o"
	@echo "... src/chip8.i"
	@echo "... src/chip8.s"
	@echo "... src/clock.o"
	@echo "... src/clock.i"
	@echo "... src/clock.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system


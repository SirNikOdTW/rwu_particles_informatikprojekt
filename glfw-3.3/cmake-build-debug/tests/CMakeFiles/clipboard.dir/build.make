# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = D:\Programme\JetBrains\CLion\bin\cmake\win\bin\cmake.exe

# The command to remove a file.
RM = D:\Programme\JetBrains\CLion\bin\cmake\win\bin\cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug"

# Include any dependencies generated for this target.
include tests/CMakeFiles/clipboard.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/clipboard.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/clipboard.dir/flags.make

tests/CMakeFiles/clipboard.dir/clipboard.c.obj: tests/CMakeFiles/clipboard.dir/flags.make
tests/CMakeFiles/clipboard.dir/clipboard.c.obj: tests/CMakeFiles/clipboard.dir/includes_C.rsp
tests/CMakeFiles/clipboard.dir/clipboard.c.obj: ../tests/clipboard.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/clipboard.dir/clipboard.c.obj"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\clipboard.dir\clipboard.c.obj   -c "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\tests\clipboard.c"

tests/CMakeFiles/clipboard.dir/clipboard.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/clipboard.dir/clipboard.c.i"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\tests\clipboard.c" > CMakeFiles\clipboard.dir\clipboard.c.i

tests/CMakeFiles/clipboard.dir/clipboard.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/clipboard.dir/clipboard.c.s"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\tests\clipboard.c" -o CMakeFiles\clipboard.dir\clipboard.c.s

tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.obj: tests/CMakeFiles/clipboard.dir/flags.make
tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.obj: tests/CMakeFiles/clipboard.dir/includes_C.rsp
tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.obj: ../deps/getopt.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.obj"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\clipboard.dir\__\deps\getopt.c.obj   -c "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\getopt.c"

tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/clipboard.dir/__/deps/getopt.c.i"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\getopt.c" > CMakeFiles\clipboard.dir\__\deps\getopt.c.i

tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/clipboard.dir/__/deps/getopt.c.s"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\getopt.c" -o CMakeFiles\clipboard.dir\__\deps\getopt.c.s

tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.obj: tests/CMakeFiles/clipboard.dir/flags.make
tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.obj: tests/CMakeFiles/clipboard.dir/includes_C.rsp
tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.obj: ../deps/glad_gl.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building C object tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.obj"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\clipboard.dir\__\deps\glad_gl.c.obj   -c "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\glad_gl.c"

tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/clipboard.dir/__/deps/glad_gl.c.i"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\glad_gl.c" > CMakeFiles\clipboard.dir\__\deps\glad_gl.c.i

tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/clipboard.dir/__/deps/glad_gl.c.s"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\glad_gl.c" -o CMakeFiles\clipboard.dir\__\deps\glad_gl.c.s

# Object files for target clipboard
clipboard_OBJECTS = \
"CMakeFiles/clipboard.dir/clipboard.c.obj" \
"CMakeFiles/clipboard.dir/__/deps/getopt.c.obj" \
"CMakeFiles/clipboard.dir/__/deps/glad_gl.c.obj"

# External object files for target clipboard
clipboard_EXTERNAL_OBJECTS =

tests/clipboard.exe: tests/CMakeFiles/clipboard.dir/clipboard.c.obj
tests/clipboard.exe: tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.obj
tests/clipboard.exe: tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.obj
tests/clipboard.exe: tests/CMakeFiles/clipboard.dir/build.make
tests/clipboard.exe: src/libglfw3.a
tests/clipboard.exe: tests/CMakeFiles/clipboard.dir/linklibs.rsp
tests/clipboard.exe: tests/CMakeFiles/clipboard.dir/objects1.rsp
tests/clipboard.exe: tests/CMakeFiles/clipboard.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable clipboard.exe"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\clipboard.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/clipboard.dir/build: tests/clipboard.exe

.PHONY : tests/CMakeFiles/clipboard.dir/build

tests/CMakeFiles/clipboard.dir/clean:
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && $(CMAKE_COMMAND) -P CMakeFiles\clipboard.dir\cmake_clean.cmake
.PHONY : tests/CMakeFiles/clipboard.dir/clean

tests/CMakeFiles/clipboard.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\tests" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests\CMakeFiles\clipboard.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : tests/CMakeFiles/clipboard.dir/depend

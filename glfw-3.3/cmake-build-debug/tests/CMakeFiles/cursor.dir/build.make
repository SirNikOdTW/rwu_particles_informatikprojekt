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
include tests/CMakeFiles/cursor.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/cursor.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/cursor.dir/flags.make

tests/CMakeFiles/cursor.dir/cursor.c.obj: tests/CMakeFiles/cursor.dir/flags.make
tests/CMakeFiles/cursor.dir/cursor.c.obj: tests/CMakeFiles/cursor.dir/includes_C.rsp
tests/CMakeFiles/cursor.dir/cursor.c.obj: ../tests/cursor.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/cursor.dir/cursor.c.obj"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\cursor.dir\cursor.c.obj   -c "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\tests\cursor.c"

tests/CMakeFiles/cursor.dir/cursor.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cursor.dir/cursor.c.i"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\tests\cursor.c" > CMakeFiles\cursor.dir\cursor.c.i

tests/CMakeFiles/cursor.dir/cursor.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cursor.dir/cursor.c.s"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\tests\cursor.c" -o CMakeFiles\cursor.dir\cursor.c.s

tests/CMakeFiles/cursor.dir/__/deps/glad_gl.c.obj: tests/CMakeFiles/cursor.dir/flags.make
tests/CMakeFiles/cursor.dir/__/deps/glad_gl.c.obj: tests/CMakeFiles/cursor.dir/includes_C.rsp
tests/CMakeFiles/cursor.dir/__/deps/glad_gl.c.obj: ../deps/glad_gl.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object tests/CMakeFiles/cursor.dir/__/deps/glad_gl.c.obj"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\cursor.dir\__\deps\glad_gl.c.obj   -c "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\glad_gl.c"

tests/CMakeFiles/cursor.dir/__/deps/glad_gl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cursor.dir/__/deps/glad_gl.c.i"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\glad_gl.c" > CMakeFiles\cursor.dir\__\deps\glad_gl.c.i

tests/CMakeFiles/cursor.dir/__/deps/glad_gl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cursor.dir/__/deps/glad_gl.c.s"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && D:\Programme\MingW\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\deps\glad_gl.c" -o CMakeFiles\cursor.dir\__\deps\glad_gl.c.s

# Object files for target cursor
cursor_OBJECTS = \
"CMakeFiles/cursor.dir/cursor.c.obj" \
"CMakeFiles/cursor.dir/__/deps/glad_gl.c.obj"

# External object files for target cursor
cursor_EXTERNAL_OBJECTS =

tests/cursor.exe: tests/CMakeFiles/cursor.dir/cursor.c.obj
tests/cursor.exe: tests/CMakeFiles/cursor.dir/__/deps/glad_gl.c.obj
tests/cursor.exe: tests/CMakeFiles/cursor.dir/build.make
tests/cursor.exe: src/libglfw3.a
tests/cursor.exe: tests/CMakeFiles/cursor.dir/linklibs.rsp
tests/cursor.exe: tests/CMakeFiles/cursor.dir/objects1.rsp
tests/cursor.exe: tests/CMakeFiles/cursor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable cursor.exe"
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\cursor.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/cursor.dir/build: tests/cursor.exe

.PHONY : tests/CMakeFiles/cursor.dir/build

tests/CMakeFiles/cursor.dir/clean:
	cd /d "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" && $(CMAKE_COMMAND) -P CMakeFiles\cursor.dir\cmake_clean.cmake
.PHONY : tests/CMakeFiles/cursor.dir/clean

tests/CMakeFiles/cursor.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\tests" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests" "D:\Niklas Birk\Dokumente\Programmierung\C\Informatikprojekt\glfw-3.3\cmake-build-debug\tests\CMakeFiles\cursor.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : tests/CMakeFiles/cursor.dir/depend

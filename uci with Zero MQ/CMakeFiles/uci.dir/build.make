# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = "/media/sf_UBU/UCI/uci with Zero MQ"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/media/sf_UBU/UCI/uci with Zero MQ"

# Include any dependencies generated for this target.
include CMakeFiles/uci.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/uci.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/uci.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/uci.dir/flags.make

CMakeFiles/uci.dir/main.cpp.o: CMakeFiles/uci.dir/flags.make
CMakeFiles/uci.dir/main.cpp.o: main.cpp
CMakeFiles/uci.dir/main.cpp.o: CMakeFiles/uci.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/media/sf_UBU/UCI/uci with Zero MQ/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/uci.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uci.dir/main.cpp.o -MF CMakeFiles/uci.dir/main.cpp.o.d -o CMakeFiles/uci.dir/main.cpp.o -c "/media/sf_UBU/UCI/uci with Zero MQ/main.cpp"

CMakeFiles/uci.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uci.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/media/sf_UBU/UCI/uci with Zero MQ/main.cpp" > CMakeFiles/uci.dir/main.cpp.i

CMakeFiles/uci.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uci.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/media/sf_UBU/UCI/uci with Zero MQ/main.cpp" -o CMakeFiles/uci.dir/main.cpp.s

CMakeFiles/uci.dir/tinyxml2.cpp.o: CMakeFiles/uci.dir/flags.make
CMakeFiles/uci.dir/tinyxml2.cpp.o: tinyxml2.cpp
CMakeFiles/uci.dir/tinyxml2.cpp.o: CMakeFiles/uci.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/media/sf_UBU/UCI/uci with Zero MQ/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/uci.dir/tinyxml2.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uci.dir/tinyxml2.cpp.o -MF CMakeFiles/uci.dir/tinyxml2.cpp.o.d -o CMakeFiles/uci.dir/tinyxml2.cpp.o -c "/media/sf_UBU/UCI/uci with Zero MQ/tinyxml2.cpp"

CMakeFiles/uci.dir/tinyxml2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/uci.dir/tinyxml2.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/media/sf_UBU/UCI/uci with Zero MQ/tinyxml2.cpp" > CMakeFiles/uci.dir/tinyxml2.cpp.i

CMakeFiles/uci.dir/tinyxml2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/uci.dir/tinyxml2.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/media/sf_UBU/UCI/uci with Zero MQ/tinyxml2.cpp" -o CMakeFiles/uci.dir/tinyxml2.cpp.s

# Object files for target uci
uci_OBJECTS = \
"CMakeFiles/uci.dir/main.cpp.o" \
"CMakeFiles/uci.dir/tinyxml2.cpp.o"

# External object files for target uci
uci_EXTERNAL_OBJECTS =

uci: CMakeFiles/uci.dir/main.cpp.o
uci: CMakeFiles/uci.dir/tinyxml2.cpp.o
uci: CMakeFiles/uci.dir/build.make
uci: /usr/lib/x86_64-linux-gnu/libnorm.a
uci: /usr/lib/x86_64-linux-gnu/libprotokit.a
uci: /usr/lib/x86_64-linux-gnu/libzmq.a
uci: /usr/lib/x86_64-linux-gnu/libpgm.a
uci: /usr/lib/x86_64-linux-gnu/libsodium.a
uci: /usr/lib/x86_64-linux-gnu/libapr-1.a
uci: /usr/lib/x86_64-linux-gnu/libuuid.a
uci: /usr/lib/x86_64-linux-gnu/libssl.a
uci: /usr/lib/x86_64-linux-gnu/libgps.a
uci: /usr/lib/x86_64-linux-gnu/libdbus-1.a
uci: /usr/lib/x86_64-linux-gnu/libsystemd.a
uci: /usr/lib/x86_64-linux-gnu/libcap.a
uci: /usr/lib/x86_64-linux-gnu/libcrypto.a
uci: CMakeFiles/uci.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/media/sf_UBU/UCI/uci with Zero MQ/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable uci"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/uci.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/uci.dir/build: uci
.PHONY : CMakeFiles/uci.dir/build

CMakeFiles/uci.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/uci.dir/cmake_clean.cmake
.PHONY : CMakeFiles/uci.dir/clean

CMakeFiles/uci.dir/depend:
	cd "/media/sf_UBU/UCI/uci with Zero MQ" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/media/sf_UBU/UCI/uci with Zero MQ" "/media/sf_UBU/UCI/uci with Zero MQ" "/media/sf_UBU/UCI/uci with Zero MQ" "/media/sf_UBU/UCI/uci with Zero MQ" "/media/sf_UBU/UCI/uci with Zero MQ/CMakeFiles/uci.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/uci.dir/depend


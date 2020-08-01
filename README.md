# th09mp
Touhou Kaeidzuka ~ Phantasmagoria of Flower View Multiplayer Patch

This project is a work in progress. A lot of code is either unfinished or experimental.

# Build instructions:

This project uses RakNet and Dear ImGui libraries. Both should be located in the /lib folder. It's not necessary to build Dear ImGui, but RakNet should be built so that CMake could find the following library: "/lib/RakNet/Lib/RakNet_VS2008_LibStatic_Debug_Win32.lib"

Automated dependency management is planned for the future.

This project uses CMake to generate solution files for different versions of Visual Studio.
Install CMake first. If you have it in your PATH variable you can generate and build the project as follows:

Generate: cmake -A Win32 ..

Build: cmake --build .

In CMakeLists.txt you can use commented set_target_properties functions to set the output directory for this project.

Note that this project has to be Win32 in order for it to work.
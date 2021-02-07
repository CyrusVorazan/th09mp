# th09mp
Touhou Kaeidzuka ~ Phantasmagoria of Flower View Multiplayer Patch

This project is a work in progress. A lot of code is either unfinished or experimental.

# Build instructions:

This project uses RakNet and Dear ImGui libraries. Both are included as submodules. Therefore, run `git clone --recursive https://github.com/CyrusVorazan/th09mp` when downloading this repository.

This project uses CMake to generate solution files for different versions of Visual Studio.
Install CMake first. If you have it in your PATH variable you can generate and build the project as follows:

Generate: cmake -A Win32 ..

Build: cmake --build .

In CMakeLists.txt you can use commented set_target_properties functions to set the output directory for this project.

Note that this project has to be Win32 in order for it to work.

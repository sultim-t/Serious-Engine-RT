# Serious Engine: Ray Traced

Real-time path tracing support for Serious Engine 1.10.

## Building

### Windows

1) Prerequisites:
    * 64-bit CPU
    * GPU with ray tracing support
    * [Git](https://github.com/git-for-windows/git/releases)
    * [Vulkan SDK](https://vulkan.lunarg.com/)
    * [Visual Studio](https://www.visualstudio.com/post-download-vs?sku=community) (any version after 2013)
    * [Python 3](https://www.python.org/downloads/)

1) Clone this repository:
    * `git clone https://github.com/sultim-t/Serious-Engine-RT.git`

1) Fetch RTGL1 library submodule:
    * `git submodule update --init`

1) Build RTGL1 library:
    * The library uses CMake: `Sources/RTGL1/CMakeLists.txt`
    * Some of the configs can be found in `Sources/RTGL1/CMakeSettings.json`
    * Make sure that compiled static library `RayTracedGL1.lib` is located in either `Sources/RTGL1/Build/x64-Release` or `Sources/RTGL1/Build/x64-Debug`

1) Build shaders:
    * Run `Sources/RTGL1/Source/Shaders/GenerateShaders.py`. This script compiles SPIR-V files and places them into `Sources/RTGL1/Build`

1) Open solution `Sources/AllTFE.sln` in Visual Studio and build it. 
    * The executables and libararies will be copied into `Bin` directory (or `Bin/Debug` if you are using the Debug configuration)

### Other OS

Not supported.

## Running

This version of the engine comes with a set of resources (`SE1_10.gro`) that allow you to freely use the engine without any additional resources required. 

However, to run Serious Sam: The First Encounter Ray Traced:
1) Copy files from the original [Serious Sam: The First Encounter](https://store.steampowered.com/app/41050/Serious_Sam_Classic_The_First_Encounter/)     
    * The list of required files is on the [Releases](https://github.com/sultim-t/Serious-Engine-RT/releases) page
2) Unpack `TFE_RT_Textures.zip`
    * Archive can be downloaded from the [Releases](https://github.com/sultim-t/Serious-Engine-RT/releases) page
    * It contains additional upscaled textures and PBR textures

Note:
* When running a selected project, make sure its project settings on Debugging is set to the right command:
    * For debug: `$(SolutionDir)..\Bin\Debug\$(TargetName).exe`
    * For release: `$(SolutionDir)..\Bin\$(TargetName).exe` 
* And its working directory: `$(SolutionDir)..\`

## License

This project is licensed under the GNU GPL v2.

x64 port is based on https://github.com/rcgordon/Serious-Engine (GNU GPL v2).

Some of the code included with the engine sources is not licensed under the GNU GPL v2:

* zlib (located in `Sources/Engine/zlib`) by Jean-loup Gailly and Mark Adler
* LightWave SDK (located in `Sources/LWSkaExporter/SDK`) by NewTek Inc.
* libogg/libvorbis (located in `Sources/libogg` and `Sources/libvorbis`) by Xiph.Org Foundation

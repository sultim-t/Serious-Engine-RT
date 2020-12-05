# Serious Engine: Ray Traced

Real-time path tracing support for Serious Engine 1.10.


## Building

### Windows

Prerequisites:
* 64-bit CPU
* GPU with ray tracing support
* [Git](https://github.com/git-for-windows/git/releases)
* [Vulkan SDK](https://vulkan.lunarg.com/)
* [Visual Studio](https://www.visualstudio.com/post-download-vs?sku=community) (any version after 2013), Professional or Community edition.

Clone this repository:

`git clone https://github.com/SultimTsyrendashiev/Serious-Engine-Vk.git`

Open solution `Sources/All.sln` in Visual Studio and build it. The executables and libararies will be into `Bin` directory (or `Bin\Debug` if you are using the Debug configuration).

### Other OS

Not supported.

## Running

This version of the engine comes with a set of resources (`\SE1_10.GRO`) that allow you to freely use the engine without any additional resources required. However if you want to open or modify levels from Serious Sam Classic: The First Encounter or The Second Encounter, you will have to copy the game's resources (.GRO files) into the engine folder. You can buy the original games on Steam, as a part of a bundle with Serious Sam Revolution ( http://store.steampowered.com/app/227780 )

When running a selected project, make sure its project settings on Debugging is set to the right command:
* For debug:
    $(SolutionDir)..\Bin\Debug\$(TargetName).exe`
* For release:
    $(SolutionDir)..\Bin\$(TargetName).exe`
And its working directory:
    $(SolutionDir)..\

## License

This project is licensed under the GNU GPL v2 (see LICENSE file).

x64 port is based on https://github.com/rcgordon/Serious-Engine (GNU GPL v2).

Some of the code included with the engine sources is not licensed under the GNU GPL v2:

* zlib (located in `Sources/Engine/zlib`) by Jean-loup Gailly and Mark Adler
* LightWave SDK (located in `Sources/LWSkaExporter/SDK`) by NewTek Inc.
* libogg/libvorbis (located in `Sources/libogg` and `Sources/libvorbis`) by Xiph.Org Foundation

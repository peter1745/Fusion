![Fusion](./Resources/FusionLogo.png)

Fusion is a game engine written in C++ that I'm mainly working on as a hobby project, when I'm not working at my day job *(where I'm also working on an engine)*.

The main goal of Fusion is for me to expand my knowledge in game engine architecture, graphics programming, as well as learning more about programming in general.

Since this is a hobby project you should keep in mind that I constantly break it. And will make arbitrary choices based on personal preference, not strictly what's best practice.
I **highly** recommend that you **don't** use Fusion for anything other than checking it out!

## Building
Fusion currently supports Windows and Linux (assuming you're on the `platform/linux` branch), Mac support is not something I'm planning on adding.

Fusion uses [Premake](https://premake.github.io/) as a build configuration system, meaning Fusion in *theory* works with whatever toolchains / compilers that premake supports. However the only compilers / toolchains I've actually tested Fusion on are these:
### Compilers
- MSVC
- Clang

### Toolchains
- Visual Studio 2022[premake-dwarf.lua](Vendor%2Fpremake-dwarf.lua)
- CLion using CMake (I had some problems with loading the generated Makefiles in CLion)
- Make (using the gmake2 generator in premake)
- CMake (using [this](https://github.com/MarcasRealAccount/premake-cmake) CMake generator)

I *highly* recommend that you install premake locally by downloading the premake executable, and adding it to your `PATH` environment variable, that way you can access premake from anywhere.

### Windows
Building on Windows is pretty simple with premake if you're using Visual Studio. Just run these commands:
```bash
git clone --recursive https://github.com/peter1745/Fusion
cd Fusion
premake5.exe vs2022 --cc={msc, clang}
```

Then just open up the Fusion.sln file in Visual Studio, select the configuration you want to build (Release is recommended) and run it.


### Linux
Building on linux is slightly more complex right now, I'm currently working on improving the workflow, but you're using Linux so you might not mind.
So to build Fusion on Linux here are the commands I recommend:
```sh
git clone --recursive https://github.com/peter1745/Fusion
cd Fusion/Scripts
./Setup.sh
cd ..
premake5.exe cmake --cc={clang}

# Either run CMake directly by running:
cmake -GNinja -DCMAKE_BUILD_TYPE={Debug, Release, Distribution}
# or open the Fusion folder in something like CLion
```
The Setup.sh script will simply install any packages that are required to build Fusion on Linux. If you're not comfortable running a random script from the internet you can just open it up.
You'll see that it loads in a text file and goes through each line, and installing the packages listed in that file. You can just open up the LinuxPackages.txt file to see which packages will be installed.

Currently, the setup script only works with the `apt` package manager, I may add support for e.g pacman and Yum at a later date. But you can just modify the script to use your package manager.

On Linux you also have the option of using GNU Make to build. The commands are pretty much identical:
```sh
git clone --recursive https://github.com/peter1745/Fusion
cd Fusion/Scripts
./Setup.sh
cd ..
premake5.exe gmake2 --cc={clang}

make config={Debug, Release, Distribution}
```

If you have any issues with building Fusion please open an issue explaining what problem you're encountering!
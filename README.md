![Fusion](./Resources/FusionLogo.png)

Fusion is a game engine written in C++ that I'm mainly working on as a hobby project, when I'm not working at my day job *(where I'm also working on an engine)*.

The main goal of Fusion is for me to expand my knowledge in game engine architecture, graphics programming, as well as learning more about programming in general.

Since this is a hobby project you should keep in mind that I constantly break it. And will make arbitrary choices based on personal preference, not strictly what's best practice.
I **highly** recommend that you **don't** use Fusion for anything other than checking it out!

## Roadmap
- [ ] Finish Direct3D 11 + Direct3D 12 API abstractions (and ensure they're stable)
- [ ] Refactor actor selection system (including proper selection using mouse picking)
- [ ] Fix up editor UI a bit
- [ ] Convert textures to be assets + texture importing
- [ ] Implement a simple material system
- [ ] Implement a basic physically-based renderer
- [ ] Initial physics implementation (Consider either Jolt, PhysX 5 or custom)

## Building
Fusion uses [Premake](https://premake.github.io/) as a build configuration system, meaning Fusion in *theory* works with whatever toolchains / compilers that premake supports. However the only compilers / toolchains I've actually tested Fusion on are these:

Currently Fusion only supports Windows, which is made apparent by it only support Direct3D as a graphics API. Vulkan support is planned for the future however, and I do plan on supporting Linux down the line.

### Compilers
- MSVC
- Clang

### Toolchains
- Visual Studio 2022

### Building on Windows
Either run the `GenerateProjects.bat` file located in the `Scripts` folder, or if you have premake installed locally (and it's in your `PATH` environment variable) you can just run `premake5.exe [action] --cc=desired_compiler` from the root directory.

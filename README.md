# [Visual Studio Code](https://code.visualstudio.com/) + [CMake](https://cmake.org/) based cross-platform template for developing a [REAPER](https://www.reaper.fm/) [Plug-in Extension](https://www.reaper.fm/sdk/plugin/plugin.php)

A template for REAPER Plug-in Extension development using Visual Studio Code and CMake with presets. Developed and tested on Windows, macOS, and Linux.

## Requirements

### Windows

Install [Visual Studio](https://visualstudio.microsoft.com/vs/features/cplusplus/) with at least these components:

- C++ CMake tools for Windows
- Windows 10/11 SDK

### macOS

Install [Homebrew](https://brew.sh/) (also installs Xcode Command Line Tools), then:

```sh
brew install cmake git gersemi
```

### Linux

```sh
sudo apt install build-essential cmake gdb git ninja-build
```

## Setup

1. Install [Visual Studio Code](https://code.visualstudio.com/) and [Git](https://git-scm.com/downloads).
2. Clone this repository:

   ```sh
   git clone https://github.com/your-username/your-plugin.git
   cd your-plugin
   ```

3. Install the [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools):

   ```sh
   code --install-extension ms-vscode.cmake-tools
   ```

4. Open the workspace: `code .`
5. CMake Tools will detect `CMakePresets.json` and prompt to select a configure preset - choose the one matching your platform and toolchain (e.g. **Windows MSVC x64 Debug**).
6. Dependencies (WDL, reaper-sdk) are fetched automatically by CMake on first configure.

## Important files

### `CMakeLists.txt` and `cmake/*.cmake`

- Project configuration

### `src/*.cpp` and `src/*.h`

- Source code

## First steps

- Build with `CMake: Build` or `F7`. The plugin is installed to your REAPER `UserPlugins` folder automatically after each build.
- Start REAPER - the action (`MyUsername: MyPlugin`) should be available in the Action List (`?`).
- Triggering the action should print `hello, world` to the REAPER console.
- Switch between Debug and Release by changing the active configure preset (`CMake: Select Configure Preset` from the Command Palette or the Status Bar).
- Debug with `F5`. On first launch, edit `launch.json` and set `"program"` to your REAPER executable path, e.g. `"C:/Program Files/REAPER (x64)/reaper.exe"`.
- [VSCode C++ docs](https://code.visualstudio.com/docs/languages/cpp#_tutorials), [Microsoft C++ docs](https://docs.microsoft.com/en-us/cpp/cpp/), and the [REAPER Developer Forum](https://forum.cockos.com/forumdisplay.php?f=3) are useful resources.

## Features

- **CMake presets**: configure/build presets for Windows, macOS, and Linux
- **Dependency fetching**: WDL and reaper-sdk are fetched with `FetchContent` during configure
- **Version and build metadata**: `prepare.cmake` reads version from git tags and records commit/branch in `config.h`
- **Install step for REAPER**: build output is copied to REAPER `UserPlugins` after build
- **Tests**: unit tests use GoogleTest with CTest
- **Packaging**: CPack config for `.exe` (Windows), `.pkg` (macOS), and `.deb` (Linux) installers
- **CI workflows**: GitHub Actions build matrix, tests, and formatting checks (clang-format, gersemi)
- **Release workflow**: `release.yml` creates version tags with semantic-release and uploads draft release artifacts
- **Formatting**: `.clang-format` config and CI checks
- **Static analysis**: `.clang-tidy` config with CMake integration
- **Git hook**: `.githooks/pre-commit` runs `clang-format` and `gersemi` formatting checks before commits
- **Dependency updates**: Dependabot updates GitHub Actions dependencies
- **Signing/notarization**: macOS and Windows code signing and notarization with CPack packaging and GitHub Actions
- **Reusable project template**: REAPER-specific logic is isolated and can be removed for non-REAPER targets; in toplevel `CMakeLists.txt` change project name, replace source files in `src/` with our own, remove `include(cmake/reaper-plugin.cmake)`, swap to `add_executable(${PROJECT_NAME})`, and start working on your own project!

![image](https://i.imgur.com/N4LuyFV.gif)
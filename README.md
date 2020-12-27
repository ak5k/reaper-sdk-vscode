# [Visual Studio Code](https://code.visualstudio.com/) + [CMake](https://cmake.org/) based cross-platform template for developing a [REAPER](https://www.reaper.fm/) [Plug-in Extension](https://www.reaper.fm/sdk/plugin/plugin.php)
An entry level solution for setting up a REAPER Plug-in Extension development and build environment based on Visual Studio Code and CMake. Goal is to setup ReaScript IDE-like working environment for developing a simple scriptlike plugin, with little if any former background on C/C++ or REAPER Plug-in Extension development itself, and begin learning. Layman's documentation is included in the source files. Developed and tested on Windows 10, MacOS High Sierra 10.13.6 and Big Sur 11.1, and Ubuntu 20.04.1. Some familiarity with Visual Studio Code is beneficial. Instructions are based on clean/fresh installation.
## Installation guide
It's recommended to read all steps in advance before beginning installation.
### Windows
Install [Visual Studio Community with Develop C and C++ applications component](https://visualstudio.microsoft.com/vs/features/cplusplus/). Default installation includes MSVC compiler and CMake. Visual Studio installation can be trimmed down before installation or afterwards by cherry-picking only the necessary components from Visual Studio Installer > Individual Components section.
#### Minimal Visual Studio installation
* C++ CMake tools for Windows
* Windows SDK (e.g. Windows 10 SDK (10.0.19041.0) for Windows 10, version 2004)
### MacOS
Perhaps the easiest way is to install [Homebrew](https://brew.sh/). This will also install Apple XCode Command Line Tools. After Homebrew is installed, install CMake with `brew install cmake`. Git can also be installed from Homebrew with `brew install git`. Current MacOS versions should include [PHP](https://www.php.net/). If not, initial CMake configuration will try to install PHP from Homebrew.
### Linux
Mainstream Linux distributions usually include compiler and many of other necessary tools. On Ubuntu, command `sudo apt install build-essential cmake gdb git valgrind` installs all necessary tools.
### All platforms
* Install [Visual Studio Code](https://code.visualstudio.com/) (VSCode).
* Install [Git](https://git-scm.com/downloads), if not already installed. Microsoft's Git for Windows doesn't seem to play well with Visual Studio Code. 
* On Windows, open **Developer PowerShell (or Command Prompt) for VS**. On MacOS and Linux, use regular Terminal. Change directory to user preferred location for source repositories, or make one.
* Get [reaper-sdk-vscode](https://github.com/ak5k/reaper-sdk-vscode) files, preferably with `git clone --recursive https://github.com/ak5k/reaper-sdk-vscode.git`, or by [creating a new repository from this template](https://docs.github.com/en/free-pro-team@latest/github/creating-cloning-and-archiving-repositories/creating-a-repository-from-a-template) and cloning it to local host. Currently templates don't include submodules, in this case [WDL](https://www.cockos.com/wdl/) library.
* Change directory to `reaper-sdk-vscode/sdk/plugin`.
* Install [VSCode C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack) with `code --verbose --install-extension ms-vscode.cpptools-extension-pack`. This might take a while.
* Open new VSCode workspace by issuing command `code .` (in directory `reaper-sdk-vscode/sdk/plugin`).
* VSCode finishes installing the C/C++ Extensions pack. This is indicated in Status Bar.
* Wait, until all dependencies have been downloaded and installed. This might take a while. 
* Then quit VSCode and restart.
* If VSCode notifies about Reload, then Reload.
* If VSCode notifies to configure project 'plugin' with CMake Tools, choose Yes to configure. CMake Tools can also be set to allow always configure new C/C++ projects.
* Select appropriate build kit for platform and architecture. Visual Studio for Windows, Clang for MacOS and GCC for Linux (e.g. Visual Studio Community Release 2019 - amd64 for modern Windows PC).
* VSCode should also notify about setting up IntelliSense for current workspace. Allow this.
* If this did not happen, these can be set up by issuing VSCode Command Palette Commands (Ctrl/Cmd + Shift + P) `CMake: Configure` and `CMake: Select a Kit`, or from VSCode Status Bar. 
## Template files
### `CMakeLists.txt`
* From VSCode workspace Explorer (Ctrl/Cmd + Shift + E) open up `CMakeLists.txt` file.
* Atomistic configuration for plugin and building/compiling.
* `PLUGIN_NAME` variable is parsed and configured to header file, to setup Action Name and Command Name in REAPER, and filename for the plugin.
* By default, CMake builds the plugin into `UserPlugins` directory under REAPER resource path.
* Name for 'plugin or project' can be set by changing `set(PLUGIN_NAME "ak5k: my plugin")`.
### `main.cpp`
* `main.cpp` and header file `main.h` are meant to contain the main code of the plugin.
* [ReaScript API](https://www.reaper.fm/sdk/reascript/reascripthelp.html) functions available.
* Function `int main()` is called when plugin Action is ran from REAPER.
### `plugin.cpp`
* `plugin.cpp` and header file `plugin.h` contain parts which register code in `main.cpp` as a REAPER Plug-in Extension and a related Action.
* ReaScript style defer behavior can be enabled by setting `defer=true`.
## First steps
* By default, VSCode builds a debug version of the plugin and installs it by keyboard shortcut `F7`.
* Start REAPER, and new plugin and it's Action ("ak5k: my plugin" by default) should show up in the Actions List.
* Running the Action should result in 'Hello World' Console Message.
* [VSCode docs](https://code.visualstudio.com/docs/languages/cpp#_tutorials) and [Microsoft C++ docs](https://docs.microsoft.com/en-us/cpp/cpp/) are a helpful resource. And, of course, [ReaScript, JSFX, REAPER Plug-in Extensions, Developer Forum](https://forum.cockos.com/forumdisplay.php?f=3).
* Choosing between debug and release builds can be done with `CMake: Select Variant`.
* Debugging is launched with `F5`. First time, VSCode opens up default Launch Task configuration for debugging. Choose correct Environment and select Default Configuration. In `launch.json` file, edit the `"program":` value to match REAPER executable/binary installation path, e.g. `"program": "C:\\Program Files\\REAPER\\reaper.exe"`, escaping backslashes on Windows paths.
* [VSCode debugger](https://code.visualstudio.com/docs/cpp/cpp-debug) allows step-by-step code execution, watching variables, etc.
* On Windows, VSCode needs to be started from Developer PowerShell (or Command Prompt) for VS.
![image](https://i.imgur.com/N4LuyFV.gif)

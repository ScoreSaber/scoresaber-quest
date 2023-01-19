## Requirements
- [QPM](https://github.com/RedBrumbler/QuestPackageManager-Rust)
- [Android NDK](https://developer.android.com/ndk/downloads)
- [CMake](https://cmake.org/install/)
- [Ninja build](https://ninja-build.org/)
- [PowerShell](https://github.com/PowerShell/PowerShell)

## How to build
1. Install all the [requirements](#requirements)
1. Execute qpm restore<br>
   `qpm-rust restore`
1. Find your android ndk file and add the filepath to `ndkpath.txt`
1. Execute the build script:<br>
   UNIX: `pwsh -Command ./build.ps1`<br>
   Windows Powershell: `./build.ps1`

# dmap-het-core

**HET-Core** is a C++ library providing core Head-Eye-Tracking functionalities such as *Face Detection*, *Head Pose Estimation*, and *Gaze Direction Estimation*. The development is part of Continental's scale-up project **Driver Monitoring Algorithms as a Product (DMAP)** and intended to be integrated in the current [Interior Camera Platform](https://confluence-id.zone2.agileci.conti.de/display/PrjCAMPF21).

This repository contains the code of:

- **lib_het_core**: The Head-Eye-Tracker library.
- **app_het_core** : The test application for lib-het-core, which processes a given recording.
- **run_tests** : The application which run all the Google tests implemented for the different **lib_het_core** modules.
## References
[**Official Documentation**](https://confluence.auto.continental.cloud/x/Q8R-EQ)

[**Official Releases**](https://github.geo.conti.de/heat-icm/dmap-het-core/releases)

[**Official Versioning**](https://github.geo.conti.de/heat-icm/dmap-het-core/blob/master/VERSIONING.md)

## Modules 
The library currently contains the following modules and components: 
| Module | Component(s) |
|--------|--------------|
| HETD   | [Face Detection](https://confluence.auto.continental.cloud/x/bkoHEw)|
| HET    | [Facial Landmarks Detection](https://confluence.auto.continental.cloud/x/DB90Ew)|
| HET    | [Head Pose Estimation](https://confluence.auto.continental.cloud/x/kSV0Ew)|

## Build Environment
The source code can be compiled in the following operating systems and platform architectures:

| Operating System | Version | Architecture |
|------------------|---------|--------------|
| Linux Ubuntu     | 16.04   | x64          |
| Linux Ubuntu     | 18.04   | x64          |
| Linux Ubuntu     | 20.04   | x64          |
| Linux            | xxxxx   | ARM64        |

## Build
To build the project, **Gtest**, **Clang-9** and **CMake 3.22.2** (or a more recent version) are required. 

Steps to build ``lib_het_core``  , ``app_het_core`` and ``run_tests``: 

For ARM64 in Debug mode : 
```
cd bin/ARM64/Debug/
cmake -DCMAKE_TOOLCHAIN_FILE="../../../build/cmake_toolchain/Linaro_ARM64_compilation.cmake" -DCMAKE_BUILD_TYPE=Debug ../../..
make
```
For ARM64 in Release mode : 
```
cd bin/ARM64/Release/
cmake -DCMAKE_TOOLCHAIN_FILE="../../../build/cmake_toolchain/Linaro_ARM64_compilation.cmake" -DCMAKE_BUILD_TYPE=Release ../../..
make
```
For x86_64 in Debug mode : 
```
cd bin/x86_64/Debug/
cmake -DCMAKE_TOOLCHAIN_FILE="../../../build/cmake_toolchain/Clang_x86_64_compilation.cmake" -DCMAKE_BUILD_TYPE=Debug ../../..
make
```
For x86_64 in Release mode : 
```
cd bin/x86_64/Release/
cmake -DCMAKE_TOOLCHAIN_FILE="../../../build/cmake_toolchain/Clang_x86_64_compilation.cmake" -DCMAKE_BUILD_TYPE=Release ../../..
make
```
## Installation
To install ``app_het_core``, ``lib_het_core`` and all the needed dependencies in a custom installation directory, run the following command within the ``bin`` directory:

```
cmake --install . --prefix <path/to/install_dir>
```

## Run the application 
In order to process a raw video, run ``app_het_core`` using

```
./app_het_core --input <path/to/raw_video.raw>
```

where ``--input`` points to a RAW video file. 

For convienence, a [test file](https://github.geo.conti.de/heat-icm/dmap-het-core/blob/master/res/test_file.raw) is provided within this repository. Once executed, ``app_het_core`` generates a *CSV output file* according to the the [CSV output format](https://confluence.auto.continental.cloud/pages/viewpage.action?pageId=363795276#AlgoTeamStandaloneApplication-OutputCSVFileformat) specified by PDC CAM.

## Run the Google tests 
In order to run the Google tests from your building folder, run ``run_tests`` application using 

```
./test/run_tests
```
These application will return the status of each test (passed/failed).
## License
Copyright &copy; 2022 Continental AG and subsidaries. All rights reserved.

CONFIDENTIAL and PROPRIETARY. For INTERNAL usage only. For detailed license terms, including used third party licenses, please refer to [LICENSE](https://github.geo.conti.de/heat-icm/dmap-het-core/blob/master/LICENSE).
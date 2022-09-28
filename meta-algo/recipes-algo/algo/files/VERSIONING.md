# Versioning
Head-Eye-Tracker Library (dmap-het-core) follows [Semantic Versioning 2.0](https://semver.org/).
Each release has the form MAJOR.MINOR.PATCH, adhering to the definitions from the linked semantic versioning doc.

Given a the definition the increment is:
- MAJOR version when you make incompatible API changes,
- MINOR version when you add functionality in a backwards compatible manner, and
- PATCH version when you make backwards compatible bug fixes.
Additional labels for pre-release and build metadata are available as extensions to the MAJOR.MINOR.PATCH format.

## Current stable release version
The version number of the current stable release can be found
[here](VERSION_NUMBER).

## Release cadence
See [Release Management](https://confluence.auto.continental.cloud/display/SPACE1133/Software+Releases)

## Dependencies

|Dependency| Version|
|---|---|
|[TensorflowLite](https://www.tensorflow.org/install/lang_c?hl=fr)|2.6|
|[OpenCV](https://docs.opencv.org/4.x/)|[4.3](https://docs.opencv.org/4.3.0/d7/d9f/tutorial_linux_install.html)|
|[Cmake](https://cmake.org/)|[3.22.2](https://pytorch.org/get-started/locally/)|

The list of dependencies, their versions and their licenses can be found [here](LICENSE). The dependencies are currently store directly in the github repository in the [dep folder](dep/).  
</br>Each individual dependency license can be found in the [licenses folder](licenses/) of the repository.
# Compatibility
Compatibility and dependencies to App, Validation Tool Chain, libContiDMS, ...? 

### Version matrix
Release | Model File | ...

## Tool Compatibility
Unless otherwise noted, please use the latest released version of the tools. Most tools are backwards compatible and support multiple versions.


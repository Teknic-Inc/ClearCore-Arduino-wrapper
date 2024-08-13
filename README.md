# ClearCore-Arduino-wrapper

This repository contains the source code for the ClearCore Arduino Wrapper, which allows users to interact with a Teknic ClearCore using common Arduino functions.

Instead of interfacing directly to the processor pins, the ClearCore Arduino Wrapper uses the ClearCore libraries and provides an interface to the ClearCore connectors. This leverages all of the ClearCore library features with the ease of programming in the Arduino IDE.

Installation of the ClearCore board within the Arduino IDE should be performed using the Arduino board manager. This repository is intended for building new packages for use with the board manager.

### Installers and Resources

https://www.teknic.com/downloads/

### Example Programs

To help you quickly get started programming your ClearCore application, several ClearCore examples are included. Once you have the Teknic ClearCore selected as the active board, the Arduino File/Examples menu will include examples demonstrating how to use various features of the ClearCore, as well as the standard examples.

### Updating ClearCore-library (Teknic) Git Submodule
If it's the first time you check-out a repo you need to use --init first:

`git submodule update --init --recursive`

For git 1.8.2 or above, the option --remote was added to support updating to latest tips of remote branches:

`git submodule update --recursive --remote`

### Creating a new version for release
1. Make necessary changes to arduino wrapper code
2. Make necessary changes to ClearCore-library and pull in those changes by updating the submodule (see above)
3. Open `zipClearCore.cmd` and set the `ver` variable to your desired version (1.X.X)
4. Run `zipClearCore.cmd`
    1. This builds/compiles libClearCore and LwIP and zips up necessary packages into a file called `ClearCore-1.X.X.zip`
    2. It also hashes the zip it just created with SHA256 to be placed in the new entry in package_clearcore_index.json
5. Copy the SHA256 hash and the size in bytes to make a new entry in the `package_clearcore_index.json`
    1. Reference previous entries for syntax

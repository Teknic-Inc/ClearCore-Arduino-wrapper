# ClearCore-Arduino-wrapper

This repository contains the source code for the ClearCore Arduino Wrapper, which allows users to interact with a Teknic ClearCore using common Arduino functions.

Instead of interfacing directly to the processor pins, the ClearCore Arduino Wrapper uses the ClearCore libraries and provides an interface to the ClearCore connectors. This leverages all of the ClearCore library features with the ease of programming in the Arduino IDE.

Installation of the ClearCore board within the Arduino IDE should be performed using the Arduino board manager. This repository is intended for building new packages for use with the board manager.

### Arduino Board Installation Procedure

* From the Arduino IDE menu, open File/Preferences
* In the Additional Boards Manager URLs field, add https://www.teknic.com/files/downloads/package_clearcore_index.json (comma separated if you have multiple URLs)
* From the Arduino IDE menu, open Tools/Board/Boards Manager
* Search for ClearCore in the Boards Manager and click install for "ClearCore by Teknic, Inc."
* From the Arduino IDE Tools/Board menu, select "Teknic ClearCore"
* For Windows machines, install the USB driver 
  * https://github.com/Teknic-Inc/ClearCore-usb-driver/releases
* From the Arduino IDE Tools/Port menu, select the port for the ClearCore

### Example Programs

To help you quickly get started programming your ClearCore application, several ClearCore examples are included. Once you have the Teknic ClearCore selected as the active board, the Arduino File/Examples menu will include examples demonstrating how to use various features of the ClearCore, as well as the standard examples.

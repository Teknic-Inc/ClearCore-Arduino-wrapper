/*
 * Title: ReadWrite
 *
 * Objective:
 *    This example demonstrates how to read from and write to a file on the
 *    SD card.
 *
 * Description:
 *    This example writes to a text file on the SD card then reads the file
 *    back to the USB serial port.
 *
 * Requirements:
 * ** A micro SD card installed in the ClearCore.
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 * Last Modified: 11/25/2020
 * This is a slightly modified version of the builtin Arduino SD ReadWrite sketch.
 * This example code is in the public domain. 
 */
#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        // Wait for the USB serial port to open.
        continue; 
    }

    Serial.print("Initializing SD card...");

    if (!SD.begin()) {
        Serial.println("initialization failed!");
        while (true) {
            // We can't continue without a working SD card
            continue;
        }
    }
    Serial.println("initialization done.");

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("test.txt", FILE_WRITE);

    // If the file opened okay, write to it:
    if (myFile) {
        Serial.print("Writing to test.txt...");
        myFile.println("testing 1, 2, 3.");
        // Close the file:
        myFile.close();
        Serial.println("done.");
    } 
    else {
        // If the file didn't open, print an error:
        Serial.println("error opening test.txt");
        while (true) {
            continue;
        }
    }

    // Re-open the file for reading:
    myFile = SD.open("test.txt");
    if (myFile) {
        Serial.println("test.txt:");

        // Read from the file until there's nothing else in it:
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        // Close the file:
        myFile.close();
    } 
    else {
        // If the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }
}

void loop() {
    // Nothing happens after setup
}


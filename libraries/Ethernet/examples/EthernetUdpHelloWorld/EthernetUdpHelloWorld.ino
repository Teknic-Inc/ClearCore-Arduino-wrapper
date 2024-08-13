/*
 * Title: EthernetUdpHelloWorld
 *
 * Objective:
 *    This example demonstrates how to enable Ethernet functionality to send and
 *    receive UDP datagrams (packets).
 *
 * Description:
 *    This example will set up ethernet communications between a ClearCore and
 *    another Ethernet source (a PC or ClearCore). The example then prints the
 *    contents of the packets received to the specified serial port and sends
 *    a "Hello World" response to the sender.
 *
 * Requirements:
 * ** Setup 1 (ClearCore and a PC): The PC should be running software capable
 *    of sending and receiving UDP packets. PacketSender is highly recommended
 *    as a free, cross-platform software. Configure PacketSender to send a UDP
 *    packet to the ClearCore by specifying the IP address and port provided to
 *    Ethernet.begin(). Your firewall or network settings may need to be
 *    adjusted in order to receive the response back from the ClearCore.
 * ** Setup 2 (ClearCore to a ClearCore): A partner sketch is included at the
 *    end of this file that can be used on the other ClearCore. The MAC address
 *    and IP address values set up for each ClearCore must be unique. The remote
 *    IP address and port used in the partner sketch must match the IP address
 *    and port used to setup the ClearCore in this sketch.
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include <Ethernet.h>

// Change the MAC address and IP address below to match your ClearCore's
// MAC address and IP.
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

// The local port to listen for connections on.
unsigned int localPort = 8888;

// The maximum number of characters to receive from an incoming packet
#define MAX_PACKET_LENGTH 100
// Buffer for holding received packets.
char packetReceived[MAX_PACKET_LENGTH];

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

// Set this false if not using DHCP to configure the local IP address.
bool usingDhcp = true;

void setup() {
    // Put your setup code here, it will run once:

    // Set up serial communication at a baud rate of 9600 bps then wait up to
    // 5 seconds for a port to open.
    // Serial communication is not required for this example to run, however the
    // example will appear to do nothing without serial output.
    Serial.begin(9600);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Get the Ethernet module up and running.
    if (usingDhcp) {
        int dhcpSuccess = Ethernet.begin(mac);
        if (dhcpSuccess) {
            Serial.println("DHCP configuration was successful.");
        }
        else {
            Serial.println("DHCP configuration was unsuccessful!");
            Serial.println("Try again using a manual configuration...");
            while (true) {
                // UDP will not work without a configured IP address.
                continue;
            }
        }
    }
    else {
        Ethernet.begin(mac, ip);
    }

    // Make sure the physical link is up before continuing.
    while (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("The Ethernet cable is unplugged...");
        delay(1000);
    }

    // Begin listening on the local port for UDP datagrams
    Udp.begin(localPort);
}

// This loop will wait to receive a packet from a remote source, then reply back
// with a packet containing a "Hello, world!" message.
void loop() {
    // Put your main code here, it will run repeatedly:

    // Look for a received packet.
    int packetSize = Udp.parsePacket();

    if (packetSize > 0) {
        Serial.print("Received packet of size ");
        Serial.print(packetSize);
        Serial.println(" bytes.");

        Serial.print("Remote IP: ");

        IPAddress remote = Udp.remoteIP();
        for (int i = 0; i < 4; i++) {
            if (i < 3) {
                Serial.print(remote[i], DEC);
                Serial.print(".");
            }
            else {
                Serial.println(remote[i], DEC);
            }
        }
        Serial.print("Remote port: ");
        Serial.println(Udp.remotePort());

        // Read the packet.
        int bytesRead = Udp.read(packetReceived, MAX_PACKET_LENGTH);
        Serial.print("Packet contents: ");
        Serial.write(packetReceived, bytesRead);
        Serial.println();

        // Send a "Hello, world!" reply packet back to the sender.
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write("Hello, world!");
        Udp.endPacket();
    }

    delay(10);
}


/*

  // ---------------------------------
  // Partner ClearCore Example Sketch
  // ---------------------------------

#include <Ethernet.h>

// Change the MAC address and IP address below to match your ClearCore's
// MAC address and IP.
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE};
IPAddress ip(192, 168, 1, 178);

// The local port to listen for connections on.
unsigned int localPort = 8888;

// The remote ClearCore's IP address and port
IPAddress remoteIp(192, 168, 1, 177);
unsigned int remotePort = 8888;

// The last time you sent a packet to the remote device, in milliseconds.
unsigned long lastSendTime = 0;
// Delay between sending packets, in milliseconds
const unsigned long sendingInterval = 10*1000;

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

// Set this false if not using DHCP to configure the local IP address.
bool usingDhcp = true;

void setup() {
    // Set up serial communication at a baud rate of 9600 bps then wait up to
    // 5 seconds for a port to open.
    // Serial communication is not required for this example to run, however the
    // example will appear to do nothing without serial output.
    Serial.begin(9600);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Get the Ethernet module up and running.
    if (usingDhcp) {
        int dhcpSuccess = Ethernet.begin(mac);
        if (dhcpSuccess) {
            Serial.println("DHCP configuration was successful.");
        }
        else {
            Serial.println("DHCP configuration was unsuccessful!");
            Serial.println("Try again using a manual configuration...");
            while (true) {
                // UDP will not work without a configured IP address.
                continue;
            }
        }
    }
    else {
        Ethernet.begin(mac, ip);
    }

    // Make sure the physical link is up before continuing.
    while (Ethernet.linkStatus() == LinkOFF) {
        // Turn the BUILTIN_LED on while the physical link is down.
    }

    // Begin listening on the local port for UDP datagrams
    Udp.begin(localPort);
}

// This loop will send a packet to the remote IP and port specified every
// 10 seconds.
void loop() {

    // Wait for 10 seconds.
    if (millis() - lastSendTime > sendingInterval) {
        Udp.beginPacket(remoteIp, remotePort);
        Udp.write("Hello ClearCore.");
        Udp.endPacket();
        lastSendTime = millis();
    }

    // Keep the connection alive.
    Ethernet.maintain();
}

*/

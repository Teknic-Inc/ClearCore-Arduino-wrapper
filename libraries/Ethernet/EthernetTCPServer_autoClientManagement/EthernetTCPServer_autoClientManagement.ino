/*
 * Title: EthernetTCPServer_autoClientManagement
 *
 * Objective:
 *    This example demonstrates how to configure a ClearCore as a TCP server to 
 *    send and receive TCP datagrams (packets). 
 *    
 * Description:
 *    This example configures a ClearCore device to act as a TCP server. 
 *    This server can receive connections from another device acting as a TCP 
 *    client to exchange data over ethernet TCP. 
 *    This simple example accepts connection requests from clients, receives and
 *    prints incoming data from connected devices, and sends a simple "Hello 
 *    client" response.
 *    A partner project, EthernetTcpClientHelloWorld, is available to configure 
 *    another ClearCore as a client.
 *
 * Setup:
 * 1. Set the usingDhcp boolean as appropriate. If not using DHCP, specify static 
 *    IP and network information.
 * 2. Ensure the server and client are connected to communicate on the same network.
 *    If both devices are directly connected (as opposed to going through a switch)
 *    a ethernet crossover cable may be required. 
 * 3. It may be helpful to use another application to view serial output from 
 *    each device. PuTTY is one such application: https://www.putty.org/
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 * 
 * Copyright (c) 2022 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include <SPI.h>
#include <Ethernet.h>

// MAC address of the ClearCore
byte mac[] = {};

// The port number on the server over which packets will be sent/received 
#define PORT_NUM 8888

// The maximum number of characters to receive from an incoming packet
#define MAX_PACKET_LENGTH 100
// Buffer for holding received packets
unsigned char packetReceived[MAX_PACKET_LENGTH];

// Set usingDhcp to false to use user defined network settings
bool usingDhcp = true;

// Initialize the ClearCore as a server listening for incoming 
// client connections on specified port (8888 by default)
EthernetServer server = EthernetServer(PORT_NUM);
  
// Initialize a client object
// This object will hold a connected clients information 
// allowing the server to interact with the client 
EthernetClient client;
  
void setup(){
  // Set up serial communication between ClearCore and PC serial terminal 
  Serial.begin(9600);
  uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

  // Set connector IO0 as a digital output
  // When IO0 state is true, a LED will light on the
  // ClearCore indicating a successful connection to a client
  pinMode(IO0, OUTPUT);
  
  // Make sure the physical link is active before continuing
  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("The Ethernet cable is unplugged...");
    delay(1000);
  }
  
  // To configure with an IP address assigned via DHCP
  if (usingDhcp) {
    // Use DHCP to configure the local IP address
    bool dhcpSuccess = Ethernet.begin(mac);
    if (dhcpSuccess) {
      Serial.print("DHCP successfully assigned an IP address: ");
      Serial.println(Ethernet.localIP());
    }
    else {
      Serial.println("DHCP configuration was unsuccessful!");
      while (true) {
        // TCP will not work without a configured IP address
        continue;
      }
    }
  } else {
    // Configure with a manually assigned IP address
    
    // ClearCore MAC address
    byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    
    // Set ClearCore's IP address
    IPAddress ip = IPAddress(192, 168, 0, 100);
    Ethernet.begin(mac, ip);
    Serial.print("Assigned manual IP address: ");
    Serial.println(Ethernet.localIP());
    
    // Optionally set additional network addresses if needed
    
    //IpAddress myDns = myDns(192, 168, 1, 1);
    //IpAddress gateway = gateway(192, 168, 1, 1);
    //IpAddress netmask = subnetress(255, 255, 255, 0);

    //Ehternet.begin(mac, ip, myDns, gateway, subnet);
  }

  // Start listening for TCP connections
  server.begin();

  Serial.println("Server now listening for client connections...");
}

uint32_t startTime = millis();
// Connect to clients, and send/receive packets
void loop(){
    // Obtain a reference to a connected client with incoming data available
    // This function will only return a valid reference if the connected device has 
    // data available to read
    client = server.available();
    
    // Check if the server has returned a connected client with incoming data available
    if (client.connected()) {
      // Flash on LED if a client has sent a message
      digitalWrite(IO0, true);
      
      // Delay to allow user to see the LED
      // This example will flash the LED each time a message from a client is received
      delay(100); 
      
      // Read packet from the client
      Serial.print("Read the following from the client: ");
      while (client.available() > 0) {
        int lengthOfMessage = client.available();
        for(int i=0; i < lengthOfMessage && i != 100; i++){ 
          char currentChar = client.read();
          packetReceived[i] = currentChar;
        }
        // Send the data received from the client over a serial port
        Serial.print((char *)packetReceived);
        
        // Clear the message buffer for the next iteration of the loop
        for(int i=0; i<MAX_PACKET_LENGTH; i++){
          packetReceived[i]=NULL;
        }
      }
      Serial.println();
      
      // Send response message to client
      if (client.write("Hello client ")>0){
        Serial.println("Sent 'Hello Client' response");
      }
      else{
        Serial.println("Unable to send reply");
      }
    } else{
      // Turn off LED if a message has not been received
      digitalWrite(IO0, false);

      //Not sure how to impliment in arduino
      //if(client.ConnectionState()->state == CLOSING){
        //client.stop();
      //}
      client.stop();
      int connectionDelay = 1000;
      if(Ethernet.linkStatus() == LinkON && (millis() - startTime > connectionDelay)){
        //Serial.println(client.ConnectionState()->state);
        Serial.println("client.connected returned false and ethernet link is active");
        startTime = millis();
      }
      // Make sure the physical link is active before continuing
      while (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("The Ethernet cable is unplugged...");
        delay(1000);
      }
    }
  }

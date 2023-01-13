/*
 * Title: EthernetTcpClientHelloWorld   
 *
 * Objective:
 *    This example demonstrates how to configure a ClearCore as a TCP client to 
 *    send and receive TCP datagrams (packets).
 *
 * Description:
 *    This example configures a ClearCore device to act as a TCP client. This client 
 *    connects to another device acting as a TCP server to exchange data over 
 *    ethernet TCP. This simple example connects to a server, sends a simple "Hello server"
 *    message, and receives and prints incoming data from the server.
 *
 *    Partner projects, EthernetTcpServer_autoClientManagement and EthernetTcpServer_manualClientManagement 
 *    are available to configure another ClearCore to act as a server.
 *
 * Setup:
 * 1. Identify the IP address of the server and specify it (as serverIp) below. If 
 *    using either EthernetTcpServer example, the server IP will print to a 
 *    connected serial terminal on startup.
 * 2. Set the usingDhcp boolean as appropriate. If not using DHCP, specify static 
 *    IP and network information.
 * 3. Ensure the server and client are connected to communicate on the same network.
 *    If both devices are directly connected (as opposed to going through a switch)
 *    an ethernet crossover cable may be required. 
 * 4. It may be helpful to use another application to view serial output from 
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

// The IP address of the server to which to connect
IPAddress serverIp = IPAddress(192, 168, 2, 109);

// The port number on the server over which packets will be sent/received
#define PORT_NUM 8888

// The maximum number of characters to receive from an incoming packet
#define MAX_PACKET_LENGTH 100
// Buffer for holding received packets.
unsigned char packetReceived[MAX_PACKET_LENGTH];

// Set usingDhcp to false to use user defined network settings
bool usingDhcp = true;

// Initialize a client object
// The ClearCore will operate as a TCP client using this object
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
  // ClearCore indicating a successful connection to a server 
  pinMode(IO0, OUTPUT);

  // Make sure the physical link is active before continuing
  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("The Ethernet cable is unplugged...");
    delay(1000);
  }
  
  // Configure with an IP address assigned via DHCP
  if (usingDhcp) {
    // Use DHCP to configure the local IP address
    bool dhcpSuccess = Ethernet.begin(mac);
    if (dhcpSuccess) {
      Serial.print("DHCP successfully assigned an IP address: ");
      Serial.println(Ethernet.localIP());
    } else {
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
    //  ClearCore IP address:
    IPAddress ip(192, 168, 0, 103);

    // Set ClearCore's IP address
    Ethernet.begin(mac, ip);
    Serial.print("Assigned manual IP address: ");
    Serial.println(Ethernet.localIP());
    
    //Optionally, set additional network addresses if needed

    //IpAddress myDns = myDns(192, 168, 1, 1);
    //IpAddress gateway = gateway(192, 168, 1, 1);
    //IpAddress netmask = subnetress(255, 255, 255, 0);

    //Ehternet.begin(mac, ip, myDns, gateway, subnet);
  }
    // Attempt to connect to a server
    if (!client.connect(serverIp, PORT_NUM)) {
         Serial.println("Failed to connect to server. Retrying...");
        }
}  
      
  uint32_t startTime = millis();
  // Connect to server, and send/receive packets
  void loop(){
    
    // Make sure the physical link is active before continuing
    while (!Ethernet.linkStatus() == LinkOFF) {
      Serial.println("The Ethernet cable is unplugged...");
    }
    
    // Attempt to connect to server
    if(!client.connected()){
      // Turn off LED if the client is not connected
      //ConnectorIO0.State(false); 
      digitalWrite(IO0, false); 
      
      uint32_t connectionDelay = 1000;
      if (!client.connect(serverIp, PORT_NUM)) {
        if(millis() - startTime > connectionDelay){
          Serial.println("Failed to connect to server. Retrying...");
          startTime = millis();
        }
      }
    } else {
      // Turn on LED if client is connected
      digitalWrite(IO0, true);
      
      // If connection was successful, send and receive packets
      if( client.write("Hello server") >0)
      {
        Serial.print("Sent 'Hello server'. Response from server: ");
        bool receivedMessage = false;
        
        // Read any incoming packets from the server over the next second
        uint32_t readDelay = 1000;
        uint32_t startTime = millis();
        while((millis() - startTime) < readDelay){
          if (client.read(packetReceived, MAX_PACKET_LENGTH) > 0)
          {
            receivedMessage = true;
            Serial.print((char *)packetReceived);
            
            // Clear the message buffer for the next iteration of the loop
            for(int i=0; i<MAX_PACKET_LENGTH; i++){
              packetReceived[i]=NULL;
            }
          }
        }
        // If no packets were received, inform the user via serial message
        if (!receivedMessage){
          Serial.println("Didn't receive message.");
        }else{
          Serial.println();
        }
      }
      else
      {
        Serial.println("Hit client.stop()");
        client.stop();
      }
    }
  }

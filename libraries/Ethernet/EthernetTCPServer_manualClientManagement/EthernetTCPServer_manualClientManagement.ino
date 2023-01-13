/*
 * Title: EthernetTCPServer_manualClientManagement 
 *
 * Objective:
 *    This example demonstrates how to configure a ClearCore as a TCP server to 
 *    send and receive TCP datagrams (packets).
 *    
 * Description:
 *    This example configures a ClearCore device to act as a TCP server. 
 *    This server can receive connections from several other devices acting as TCP 
 *    clients to exchange data over ethernet TCP. 
 *    This simple example accepts connection requests from clients, checks for
 *    incoming data from connected devices, and sends a simple "Hello 
 *    client" response.
 *    A partner project, EthernetTcpClientHelloWorld, is available to configure 
 *    another ClearCore as a client.  
 *
 * Setup:
 * 1. Set the usingDhcp boolean as appropriate. If not using DHCP, specify static 
 *    IP and network information.
 * 2. Ensure the server and client are setup to communicate on the same network.
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

//#include <ClearCore.h>
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
// Buffer for holding packets to send
char packetToSend[MAX_PACKET_LENGTH]; 

// Set total number of clients the server will accept
#define NUMBER_OF_CLIENTS 6

// Set usingDhcp to false to use user defined network settings
bool usingDhcp = true;

// Initialize the ClearCore as a server 
// Clients connect on specified port (8888 by default)
EthernetServer server = EthernetServer(PORT_NUM);
  
// Initialize an array of clients
// This array holds the information of any client which
// is currently connected to the server and is used by the
// server to interact with these clients
EthernetClient clients[NUMBER_OF_CLIENTS];
// Initialize an temporary client for client management
EthernetClient tempClient;
  
// Array of output LEDs to indicate client connections
// NOTE: only connectors IO0 through IO5 can be configured as digital outputs (LEDs) 
pin_size_t outputLEDs[6] = {
  IO0, IO1, IO2, IO3, IO4, IO5
};


void setup() {
  // Set up serial communication between ClearCore and PC serial terminal 
  Serial.begin(9600);
  uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

  // Set connectors IO0-IO5 as a digital outputs 
  // When an outputLEDs state is true, a LED will light on the
  // ClearCore indicating a successful connection to a client
  for(int i=0; i<6; i++){
  pinMode(outputLEDs[i], OUTPUT);
  }
  
  // Make sure the physical link is active before continuing
  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("The Ethernet cable is unplugged...");
    delay(1000);
  }
  
  //To configure with an IP address assigned via DHCP
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

  bool newClient = 0;
  
  // Resets a timmer used to display a list of connected clients
  uint32_t startTime = 0;
  
  // Connect to clients, and send/receive packets
void loop(){
    
    // Obtain a reference to a connected client
    // This function returns a specific client once per connection attempt
    // To maintain a connection with a client using Accept() 
    // some basic client management must be performed
    tempClient = server.accept();
    
    // Checks if server.Accept() has returned a new client
    if(tempClient.connected()){
      newClient = 1; 
      
      for(int i=0; i<NUMBER_OF_CLIENTS; i++){
        //Checks for an available location to store a new client
        if(!clients[i].connected()){
          clients[i].stop();
          clients[i] = tempClient;
          Serial.print(clients[i].remoteIP());
          Serial.println(" has been connected");
          newClient = 0;
          break;
        }
      }
      // Rejects client if the client list is full 
      if(newClient == 1){
        newClient = 0;
        tempClient.write("This server has reached its max number of clients. Closing connection.");
        Serial.println("This server has reached its max number of clients. Closing connection.");
        tempClient.stop();
      }
    }
    
    // Loops through list of clients to receive/send messages
    for(int i=0; i<NUMBER_OF_CLIENTS; i++){
      if (clients[i].connected()) {
        
        // Indicate connection on corresponding output LED
        digitalWrite(outputLEDs[i], true);
        
        // Check if client has incoming data available  
        if(clients[i].available()){
          Serial.print("Read the following from the client(");
          Serial.print(clients[i].remoteIP());
          Serial.print("): ");
          // Read packet from the client
          while (clients[i].available()) {
            // Send the data received from the client over a serial port
            clients[i].read(packetReceived, MAX_PACKET_LENGTH);
            Serial.print((char *)packetReceived);
            // Clear the message buffer for the next iteration of the loop
            for(int i=0; i<MAX_PACKET_LENGTH; i++){
              packetReceived[i]=NULL;
            }
          }
          Serial.println();
          
          //Sends unique response to the client
          if(clients[i].write("Hello client ") == 0){
            // If the message was unable to send close the client connection
            clients[i].stop();
            Serial.print("Client ("); 
            Serial.print(clients[i].remoteIP());
            Serial.print(") has been removed from client list. ");
            clients[i] = EthernetClient();
            
            // Indicate loss of connection by turning off corresponding LED
            digitalWrite(outputLEDs[i], false);
          }

        }
        
      } else{
        // Removes any disconnected clients
        if(clients[i].remoteIP() != IPAddress(0,0,0,0)){
          Serial.print("Client (");
          Serial.print(clients[i].remoteIP());
          Serial.print(") has been removed from client list. ");
          clients[i].stop();
          clients[i] = EthernetClient();

          // Indicate loss of connection by turning off corresponding LED
          digitalWrite(outputLEDs[i], false);
        }
      }
    }
    
    // Make sure the physical link is active before continuing
    while (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("The Ethernet cable is unplugged...");
      delay(1000);
    }
    
    // Print out a list of current clients
    int delay = 5000;
    if(millis()- startTime > delay){
      Serial.println("List of current clients: ");
      for(int i=0; i<NUMBER_OF_CLIENTS; i++){
        if(clients[i].remoteIP() != IPAddress(0,0,0,0)){
          Serial.print("Client ");
          Serial.print(i);
          Serial.print(" = ");
          Serial.println(clients[i].remoteIP());
        }
      }
      startTime = millis();
    }
  }

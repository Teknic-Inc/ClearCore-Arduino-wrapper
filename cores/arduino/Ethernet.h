/* Copyright 2018 Paul Stoffregen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ethernet_h_
#define ethernet_h_

// All symbols exposed to Arduino sketches are contained in this header file
//
// Older versions had much of this stuff in EthernetClient.h, EthernetServer.h,
// and socket.h.  Including headers in different order could cause trouble, so
// these "friend" classes are now defined in the same header file.  socket.h
// was removed to avoid possible conflict with the C library header files.

#include <Arduino.h>
#include "EthernetUdp.h"
#include "EthernetTcp.h"
#include "EthernetTcpClient.h"
#include "EthernetTcpServer.h"
#include "Client.h"
#include "Server.h"
#include "lwip/ip_addr.h"

enum EthernetLinkStatus {
    Unknown,
    LinkON,
    LinkOFF
};

enum EthernetHardwareStatus {
    EthernetNoHardware, // No WIZnet hardware...
    EthernetW5100,
    EthernetW5200,
    EthernetW5500,
    EthernetBuiltIn
};

class EthernetUDP;
class EthernetClient;
class EthernetServer;
class DhcpClass;

class EthernetClass {
public:
    // Initialize the Ethernet shield to use the provided MAC address and
    // gain the rest of the configuration through DHCP.
    // Returns 0 if the DHCP configuration failed, and 1 if it succeeded.
    int begin(uint8_t *mac);
    int maintain();
    EthernetLinkStatus linkStatus();
    EthernetHardwareStatus hardwareStatus();

    // Manual configuration
    void begin(uint8_t *mac, IPAddress ip);
    void begin(uint8_t *mac, IPAddress ip, IPAddress dns);
    void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway);
    void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway,
               IPAddress subnet);
    void init(uint8_t sspin);

    void MACAddress(uint8_t *mac_address);
    IPAddress localIP();
    IPAddress subnetMask();
    IPAddress gatewayIP();
    IPAddress dnsServerIP();

    void setMACAddress(const uint8_t *mac);
    void setLocalIP(const IPAddress local_ip);
    void setSubnetMask(const IPAddress subnet);
    void setGatewayIP(const IPAddress gateway);
    void setDnsServerIP(const IPAddress dns);
    void setRetransmissionTimeout(uint16_t milliseconds);
    void setRetransmissionCount(uint8_t number);

    // Convert an lwIP IP address type to an Arduino IP address type (IPv4).
    IPAddress convertIp(ip_addr_t *ip);

    // Convert an Arduino IP address type to an lwIP IP address type (IPv4).
    ip_addr_t convertIp(IPAddress ip);

    friend class EthernetClient;
    friend class EthernetServer;
    friend class EthernetUDP;
};

extern EthernetClass Ethernet;

class EthernetUDP : public UDP {
public:
    EthernetUDP();
    // Initialize the UDP library and network settings.
    // Start listening on the localPort. Returns 1 if successful,
    // 0 if there are no sockets available to use.
    virtual uint8_t begin(uint16_t localPort);

    // initialize, start listening on specified multicast IP address and port.
    // Returns 1 if successful, 0 on failure.
    virtual uint8_t beginMulticast(IPAddress, uint16_t) {
        return 0;
    }

    // Disconnect from the server. Release any resource being used during the
    // UDP session.
    virtual void stop();

    // Sending UDP datagrams

    // Start a connection to write UDP data to the remote connection given by
    // remoteIp and remotePort. Returns 1 if successful, 0 if there was a
    // problem resolving the IP or port.
    virtual int beginPacket(IPAddress remoteIp, uint16_t remotePort);
    // Use DNS to resolve the given hostname to an IP address then start
    // a UDP connection.
    virtual int beginPacket(const char *host, uint16_t remotePort);
    // Call after writing UDP data to the remote connection with write() to send
    // the UDP packet started with beginPacket(). Returns 1 if the packet was
    // sent successfully, 0 if there was an error.
    virtual int endPacket();

    // Write UDP data to the outgoing packet set up with beginPacket(). The
    // packet will not be sent until endPacket() is called.
    // Returns the actual number of bytes written.

    // Write a single byte into the outgoing packet.
    virtual size_t write(uint8_t);
    // Write size bytes from the buffer into the outgoing packet.
    virtual size_t write(const uint8_t *buffer, size_t size);

    using Print::write;

    // Receiving UDP datagrams

    // Check for the presence of a UDP packet and return the size in bytes.
    // parsePacket() must be called before reading the packet with read().
    virtual int parsePacket();
    // The number of bytes available for reading from the buffer.
    // parsePacket() must be called before calling available().
    virtual int available();

    // Read UDP data. Returns the number of bytes read.
    // parsePacket() must be called before read().

    // Read one byte/character from the received packet.
    virtual int read();
    // Read up to len bytes from the received packet and put them into buffer.
    virtual int read(unsigned char *buffer, size_t len);
    virtual int read(char *buffer, size_t len) {
        return read((unsigned char *)buffer, len);
    };
    // Return the next byte from the current packet without moving on to the
    // next byte.
    virtual int peek();
    // Finish reading the current packet.
    virtual void flush();

    // The IP address of the remote connection. Must be called after parsePacket().
    virtual IPAddress remoteIP();
    // The port of the remote connection. Must be called after parsePacket().
    virtual uint16_t remotePort();

private:
    ClearCore::EthernetUdp m_udp;
};

class EthernetServer : public Server {
public:
    // Create a server that listens for incoming connections on the specified port.
    EthernetServer(uint16_t port = 80);

    // Tell the server to begin listening for incoming connections.
    virtual void begin();

    // Get a client connected to the server that has data available for reading.
    // The connection persists; close it by calling stop() on the client.
    EthernetClient available();

    // Get a client connected to the server. The caller must keep track of the
    // returned client, since accept() will return it only once.
    virtual EthernetClient accept();

    // Write data to all the clients connected to a server.
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);

    // This allows you to use "if (server)" to check if the supplied
    // EthernetServer server is started and connected.
    virtual operator bool();

    using Print::write;

private:
    ClearCore::EthernetTcpServer m_tcpServer;
};

class EthernetClient : public Client {
public:
    // Create a client which can connect to a specified internet IP address
    // and port (defined via connect()).
    EthernetClient();
    EthernetClient(ClearCore::EthernetTcpClient tcpClient);

    // Connects to a specified IP address and port. Supports DNS lookups
    // when using a domain name.
    // Return value indicates connection status:
    //  1: SUCCESS
    // -1: TIMED_OUT
    // -2: INVALID_SERVER
    // -3: TRUNCATED
    // -4: INVALID_RESPONSE
    virtual int connect(IPAddress ip, uint16_t port);
    virtual int connect(const char *host, uint16_t port);

    // Write data to the server the client is connected to.
    virtual size_t write(uint8_t val);
    virtual size_t write(const uint8_t *buf, size_t size);

    // Return the number of bytes available for reading (the amount of data that
    // has been written to the client by the server it is connected to).
    virtual int available();

    // Read the next byte received from the server the client is connected to.
    virtual int read();
    // Read size bytes into the supplied buf from the associated server.
    virtual int read(uint8_t *buf, size_t size);

    // Wait until all outgoing characters in buffer have been sent.
    virtual void flush();
    // Disconnect from the server.
    virtual void stop();
    // Return whether the client is connected. (The client is still considered
    // connected if the connection has been closed but there is still unread
    // data.)
    virtual uint8_t connected();
    // Peek at the first character received from the server.
    virtual int peek();

    // Return the local port number the client is connected to.
    virtual uint16_t localPort();
    // Return the IP address of the client. (Not the server's IP?)
    virtual IPAddress remoteIP();
    // Return the remote port number of the server the client is connected to.
    virtual uint16_t remotePort();
    // Set the timeout for connect() and stop(), in milliseconds. The initial
    // value is 1000 ms.
    virtual void setConnectionTimeout(uint16_t milliseconds);

    virtual operator bool();
    virtual bool operator==(const bool value) {
        return bool() == value;
    }
    virtual bool operator!=(const bool value) {
        return bool() != value;
    }
    virtual bool operator==(const EthernetClient &);
    virtual bool operator!=(const EthernetClient &rhs) {
        return !this->operator==(rhs);
    };

    friend class EthernetServer;

    using Print::write;
protected:
    ClearCore::EthernetTcpClient m_tcpClient;
    bool m_dnsInitialized;
};

#endif

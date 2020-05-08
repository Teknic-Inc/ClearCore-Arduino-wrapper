#include <Ethernet.h>

EthernetServer::EthernetServer(uint16_t port/* = 80*/) : m_tcpServer(port) {}

void EthernetServer::begin() {
    m_tcpServer.Begin();
}

// EthernetServer gives a client only once, regardless of it it has sent data.
// Then, the user is responsible for keeping track of connected clients.
EthernetClient EthernetServer::accept() {
    return EthernetClient(m_tcpServer.Accept());
}

// EthernetServer manages the clients. A client is only identified and returned
// when data has been received from the client and is available for reading.
EthernetClient EthernetServer::available() {
    return EthernetClient(m_tcpServer.Available());
}

size_t EthernetServer::write(uint8_t c) {
    return write(&c, 1);
}

size_t EthernetServer::write(const uint8_t *buf, size_t size) {
    return m_tcpServer.Send(buf, size);
}

EthernetServer::operator bool() {
    return m_tcpServer.Ready();
}
#include <Ethernet.h>
#include "EthernetUtils.h"
#include "IpAddress.h"

EthernetClient::EthernetClient()
    : m_tcpClient(),
      m_dnsInitialized(false) {}

EthernetClient::EthernetClient(ClearCore::EthernetTcpClient tcpClient)
    : m_tcpClient(tcpClient),
      m_dnsInitialized(false) {}

int EthernetClient::connect(IPAddress ip, uint16_t port) {
    ClearCore::IpAddress nativeIp = ClearCore::IpAddress(uint32_t(ip));
    return m_tcpClient.Connect(nativeIp, port) ? 1 : 0;
}

int EthernetClient::connect(const char *host, uint16_t port) {
    int result = -1;
    // Attempt to resolve the host.
    ip_addr_t remoteIp = {};
    err_t err = ClearCore::DnsGetHostByName(host, &remoteIp);
    IPAddress ip = Ethernet.convertIp(&remoteIp);

    switch (err) {
        case ERR_OK:
            if (connect(ip, port) == 1) {
                result = 1; // SUCCESS
            }
            else {
                result = -4; // INVALID_RESPONSE
            }
            break;
        case ERR_INPROGRESS:
            result = -1; // TIMED_OUT
            break;
        case ERR_VAL:
            result = -2; // INVALID_SERVER
            break;
        case ERR_MEM:
            result = -3; // TRUNCATED
            break;
        case ERR_ARG:
        default:
            result = -4; // INVALID_RESPONSE
            break;
    }
    return result;
}

size_t EthernetClient::write(uint8_t c) {
    return write(&c, 1);
}

size_t EthernetClient::write(const uint8_t *buffer, size_t size) {
    return m_tcpClient.Send(buffer, size);
}

int EthernetClient::available() {
    return m_tcpClient.BytesAvailable();
}

int EthernetClient::read() {
    return m_tcpClient.Read();
}

int EthernetClient::read(uint8_t *buf, size_t size) {
    return m_tcpClient.Read(buf, size);
}

// wait until all outgoing data to the client has been sent
void EthernetClient::flush() {
    m_tcpClient.Flush();
}

void EthernetClient::stop() {
    m_tcpClient.Close();
}

// A client is considered connected if the connection has been closed but
// there is still unread data.
uint8_t EthernetClient::connected() {
    if (m_tcpClient.BytesAvailable() > 0 || m_tcpClient.Connected()) {
        return 1;
    }
    return 0;
}

int EthernetClient::peek() {
    return m_tcpClient.Peek();
}

uint16_t EthernetClient::localPort() {
    return m_tcpClient.LocalPort();
}

IPAddress EthernetClient::remoteIP() {
    IPAddress ip = IPAddress(uint32_t(m_tcpClient.RemoteIp()));
    return ip;
}

uint16_t EthernetClient::remotePort() {
    return m_tcpClient.RemotePort();
}

void EthernetClient::setConnectionTimeout(uint16_t milliseconds) {
    m_tcpClient.ConnectionTimeout(milliseconds);
}
EthernetClient::operator bool() {
    return m_tcpClient.ConnectionState() != nullptr;
}

bool EthernetClient::operator==(const EthernetClient &client) {
    return m_tcpClient == client.m_tcpClient;
}
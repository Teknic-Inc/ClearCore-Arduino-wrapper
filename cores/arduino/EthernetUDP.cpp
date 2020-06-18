#include <Ethernet.h>
#include "EthernetUdp.h"
#include "EthernetUtils.h"
#include "IpAddress.h"


EthernetUDP::EthernetUDP(): m_udp() {}

uint8_t EthernetUDP::begin(uint16_t localPort) {
    return m_udp.Begin(localPort) ? 1 : 0;
}

void EthernetUDP::stop() {
    m_udp.End();
}

int EthernetUDP::beginPacket(IPAddress ip, uint16_t port) {
    ClearCore::IpAddress nativeIp = ClearCore::IpAddress(uint32_t(ip));
    return m_udp.Connect(nativeIp, port) ? 1 : 0;
}

int EthernetUDP::beginPacket(const char *host, uint16_t remotePort) {
    // Attempt to resolve the host.
    ip_addr_t remoteIp = {};
    if (ClearCore::DnsGetHostByName(host, &remoteIp) != ERR_OK) {
        return 0; // Unable to resolve hostname or timed out.
    }

    IPAddress ip = Ethernet.convertIp(&remoteIp);

    return beginPacket(ip, remotePort);
}

int EthernetUDP::endPacket() {
    return m_udp.PacketSend() ? 1 : 0;
}

size_t EthernetUDP::write(uint8_t c) {
    return write(&c, 1);
}

size_t EthernetUDP::write(const uint8_t *buffer, size_t size) {
    return m_udp.PacketWrite(buffer, size);
}

int EthernetUDP::parsePacket() {
    return m_udp.PacketParse();
}

int EthernetUDP::available() {
    return m_udp.BytesAvailable();
}

int EthernetUDP::read() {
    unsigned char c;
    return (read(&c, 1) == 1) ? c : -1;
}

int EthernetUDP::read(unsigned char *buffer, size_t len) {
    return m_udp.PacketRead(buffer, len);
}

int EthernetUDP::peek() {
    return m_udp.Peek();
}

void EthernetUDP::flush() {
    m_udp.PacketFlush();
}

IPAddress EthernetUDP::remoteIP() {
    IPAddress ip = IPAddress(uint32_t(m_udp.RemoteIp()));
    return ip;
}

uint16_t EthernetUDP::remotePort() {
    return m_udp.RemotePort();
}
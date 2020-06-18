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

#include <Arduino.h>
#include "Ethernet.h"
#include "EthernetManager.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

namespace ClearCore {
extern EthernetManager &EthernetMgr;
}

int EthernetClass::begin(uint8_t *mac) {
    (void)mac; // Disallow setting the MAC address.

    ClearCore::EthernetMgr.Setup();

    int dhcpSuccess = ClearCore::EthernetMgr.DhcpBegin();
    if (dhcpSuccess) {
        const ip_addr_t *dns = dns_getserver(0);
        ClearCore::EthernetMgr.DnsIp(dns->addr);
    }
    return dhcpSuccess;
}

// Begin functions assume that the DNS server and/or gateway will be on the
// same network with the provided IP with the last value being 1.
void EthernetClass::begin(uint8_t *mac, IPAddress ip) {
    IPAddress dns = IPAddress(ip[0], ip[1], ip[2], 1);
    begin(mac, ip, dns);
}

void EthernetClass::begin(uint8_t *mac, IPAddress ip, IPAddress dns) {
    IPAddress gateway = IPAddress(ip[0], ip[1], ip[2], 1);
    begin(mac, ip, dns, gateway);
}

void EthernetClass::begin(uint8_t *mac, IPAddress ip, IPAddress dns,
                          IPAddress gateway) {
    IPAddress subnet(255, 255, 255, 0);
    begin(mac, ip, dns, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress ip, IPAddress dns,
                          IPAddress gateway, IPAddress subnet) {
    (void)mac; // Disallow setting the MAC address.

    ClearCore::EthernetMgr.Setup();

    ClearCore::EthernetMgr.LocalIp(
        ClearCore::IpAddress(uint32_t(ip)));
    ClearCore::EthernetMgr.GatewayIp(
        ClearCore::IpAddress(uint32_t(gateway)));
    ClearCore::EthernetMgr.NetmaskIp(
        ClearCore::IpAddress(uint32_t(subnet)));
    ClearCore::EthernetMgr.DnsIp(
        ClearCore::IpAddress(uint32_t(dns)));
}

void EthernetClass::init(uint8_t sspin) {
    (void)sspin; // Nothing to implement here.
}

EthernetLinkStatus EthernetClass::linkStatus() {
    return ClearCore::EthernetMgr.PhyLinkActive() ? LinkON : LinkOFF;
}

EthernetHardwareStatus EthernetClass::hardwareStatus() {
    return EthernetBuiltIn;
}

int EthernetClass::maintain() {
    ClearCore::EthernetMgr.Refresh();
    // lwIP handles all DHCP state changes (renewal, rebind, etc.)
    // in the background via sys_check_timeouts()
    return 0;
}

void EthernetClass::MACAddress(uint8_t *mac_address) {
    memcpy(mac_address, ClearCore::EthernetMgr.MacAddress(), 6);
}

IPAddress EthernetClass::localIP() {
    return IPAddress(uint32_t(ClearCore::EthernetMgr.LocalIp()));
}

IPAddress EthernetClass::subnetMask() {
    return IPAddress(uint32_t(ClearCore::EthernetMgr.NetmaskIp()));
}

IPAddress EthernetClass::gatewayIP() {
    return IPAddress(uint32_t(ClearCore::EthernetMgr.GatewayIp()));
}

IPAddress EthernetClass::dnsServerIP() {
    return IPAddress(uint32_t(ClearCore::EthernetMgr.DnsIp()));
}

void EthernetClass::setMACAddress(const uint8_t *mac) {
    (void)mac; // Disallow setting the MAC address.
}

void EthernetClass::setLocalIP(const IPAddress local_ip) {
    ClearCore::EthernetMgr.LocalIp(uint32_t(local_ip));
}

void EthernetClass::setSubnetMask(const IPAddress subnet) {
    ClearCore::EthernetMgr.NetmaskIp(uint32_t(subnet));
}

void EthernetClass::setGatewayIP(const IPAddress gateway) {
    ClearCore::EthernetMgr.GatewayIp(uint32_t(gateway));
}

void EthernetClass::setDnsServerIP(const IPAddress dns) {
    ClearCore::EthernetMgr.DnsIp(uint32_t(dns));
}

void EthernetClass::setRetransmissionTimeout(uint16_t milliseconds) {
    milliseconds = constrain(milliseconds, 1, 6553);
    ClearCore::EthernetMgr.RetransmissionTimeout(milliseconds);
}

void EthernetClass::setRetransmissionCount(uint8_t number) {
    if (number == 0) {
        number = 1; // We should try at least once.
    }

    ClearCore::EthernetMgr.RetransmissionCount(number);
}

IPAddress EthernetClass::convertIp(ip_addr_t *ip) {
    IPAddress ipAddr = IPAddress(ip->addr);
    return ipAddr;
}

ip_addr_t EthernetClass::convertIp(IPAddress ip) {
    ip_addr_t ipAddr = {};
    IP4_ADDR(&ipAddr, ip[0], ip[1], ip[2], ip[3]);
    return ipAddr;
}

EthernetClass Ethernet;
#include "defines.h"

#define ASYNC_UDP_ESP32_SC_W6100_DEBUG_PORT LOGSerial
#define _ASYNC_UDP_ESP32_SC_W6100_LOGLEVEL_ 4

#include "ethernet_test.h"
#include <Arduino.h>
#include <AsyncUDP_ESP32_SC_W6100.h>

const uint8_t macAddress[] = MAC_ADDR;

#include <SPI.h>
#include <ctime>

IPAddress ntpServer(208, 81, 1, 244);

#define NTP_PORT 123

constexpr int PACKET_SIZE = 48;
byte buffer[PACKET_SIZE];

AsyncUDP udpClient;

void resetWizchip() {
    digitalWrite(W6100_RESET_GPIO, LOW);
    delay(100);
    digitalWrite(W6100_RESET_GPIO, HIGH);
    delay(100);
}

void constructNTPRequest() {
    LOGSerial.println("Building NTP request packet");
    memset(buffer, 0, PACKET_SIZE);
    buffer[0] = 0b11100011;
    buffer[1] = 0;
    buffer[2] = 6;
    buffer[3] = 0xEC;
    buffer[12] = 49;
    buffer[13] = 0x4E;
    buffer[14] = 49;
    buffer[15] = 52;
}

void processIncomingPacket(AsyncUDPPacket packet) {
    struct tm timeStruct;
    char timeString[80];

    memcpy(buffer, packet.data(), sizeof(buffer));
    
    LOGSerial.printf("Received UDP Packet Type: %s\n", packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
    LOGSerial.printf("From: %s:%d, To: %s:%d, Length: %d\n",
                     packet.remoteIP().toString().c_str(), packet.remotePort(),
                     packet.localIP().toString().c_str(), packet.localPort(),
                     packet.length());

    unsigned long highBytes = word(buffer[40], buffer[41]);
    unsigned long lowBytes = word(buffer[42], buffer[43]);
    unsigned long ntpTime = (highBytes << 16) | lowBytes;

    LOGSerial.printf("NTP Time (since 1900): %lu\n", ntpTime);

    constexpr unsigned long unixOffset = 2208988800UL;
    time_t unixTime = ntpTime - unixOffset;
    LOGSerial.printf("Unix Epoch Time: %lu\n", unixTime);

    timeStruct = *localtime(&unixTime);
    strftime(timeString, sizeof(timeString), "%a %Y-%m-%d %H:%M:%S %Z", &timeStruct);
    LOGSerial.println(timeString);
}

void sendNTPRequest() {
    constructNTPRequest();
    udpClient.write(buffer, sizeof(buffer));
}

[[noreturn]] void ethernetTask(void *params) {
    LOGSerial.println("Starting Ethernet task");
    LOGSerial.setDebugOutput(true);

    pinMode(W6100_CS_GPIO, OUTPUT);
    pinMode(W6100_RESET_GPIO, OUTPUT);
    SPI.begin(W6100_SCK_GPIO, W6100_MISO_GPIO, W6100_MOSI_GPIO, W6100_CS_GPIO);

    resetWizchip();

    LOGSerial.println("Initializing Ethernet...");
    ESP32_W6100_onEvent();
    ETH.begin(W6100_MISO_GPIO, W6100_MOSI_GPIO, W6100_SCK_GPIO, W6100_CS_GPIO, W6100_INT_GPIO, SPI_CLOCK_MHZ, SPI2_HOST, const_cast<uint8_t *>(macAddress));
    ESP32_W6100_waitForConnect();

    LOGSerial.printf("Async UDP NTP Client active at: %s\n", ETH.localIP().toString().c_str());
    
    if (udpClient.connect(ntpServer, NTP_PORT)) {
        LOGSerial.println("UDP connection established");
        udpClient.onPacket([](AsyncUDPPacket packet) { processIncomingPacket(packet); });
        while (true) {
            sendNTPRequest();
            delay(60000);
        }
    }
    LOGSerial.println("Failed to connect to NTP server");
    while (true) {
        delay(1000);
    }
}

void init_ethernet_test() {
    xTaskCreatePinnedToCore(ethernetTask, "ethernetTask", 2048, nullptr, 1, nullptr, 1);
}

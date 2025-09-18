#include "SendArtPollReply.h"
#include "ConfigManager.h"
#include <Arduino.h>
#include <IPAddress.h>

static const uint16_t ARTNET_PORT = 6454;
static const uint16_t OpPollReply = 0x2100;

void sendArtPollReply(UDP* udp) {
    const size_t REPLY_LEN = 64; 
    uint8_t buf[REPLY_LEN] = {0};
    memcpy(buf, "Art-Net\0", 8);
    buf[8]  = OpPollReply & 0xFF;
    buf[9]  = (OpPollReply >> 8) & 0xFF;
    buf[10] = 0; buf[11] = 14; // ProtVer Hi/Lo

    // IP address
    IPAddress ip = ConfigManager::getConfig().localIP;
    buf[14] = ip[0]; buf[15] = ip[1]; buf[16] = ip[2]; buf[17] = ip[3];

    // Port
    buf[18] = (ARTNET_PORT >> 8) & 0xFF;
    buf[19] = ARTNET_PORT & 0xFF;

    // Short name (17 chars)
    const char* name = Config::deviceName;
    strncpy((char*)&buf[26], name, 17);

    // Broadcast to /24
    IPAddress broadcast(ip[0], ip[1], ip[2], 255);
    udp->beginPacket(broadcast, ARTNET_PORT);
    udp->write(buf, REPLY_LEN);
    udp->endPacket();
    
    Serial.printf("[SendArtPollReply] Broadcast ArtPollReply to %s (%u bytes)\n", broadcast.toString().c_str(), REPLY_LEN);
}
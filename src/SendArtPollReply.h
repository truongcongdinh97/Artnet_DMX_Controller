#pragma once
#include <Arduino.h>
#include <Udp.h>

// Sends an ArtPollReply over the provided UDP interface
// The reply is broadcast to the local subnet (assumes /24).
void sendArtPollReply(UDP* udp);
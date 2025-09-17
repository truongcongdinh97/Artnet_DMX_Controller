#pragma once
#include <Arduino.h>
#include <Udp.h>

namespace ArtnetHandler {
  typedef void (*DmxPacketCallback)(uint16_t universe, uint16_t length, const uint8_t* data);
  
  // ArtnetHandler giờ sẽ nhận một đối tượng UDP đã được khởi tạo từ bên ngoài
  void begin(UDP* udp_instance);
  
  void loop();
  void setDmxPacketCallback(DmxPacketCallback cb);
}
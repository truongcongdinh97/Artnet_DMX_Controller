# ArtNet LED Controller ![Build](https://img.shields.io/badge/build-passing-brightgreen)

Điều khiển LED WS2812 qua ArtNet trên ESP32, hỗ trợ ghi phát lại, cấu hình qua WebUI, cập nhật OTA, LCD, encoder và status LED.

![demo](docs/demo.gif)

## Tính năng nổi bật
- Điều khiển LED WS2812 qua ArtNet (ESP32)
- Ghi & phát lại dữ liệu ArtNet lên thẻ SD
- WebUI cấu hình WiFi, IP, số lượng LED, OTA firmware
- Hỗ trợ LCD I2C, encoder, status LED báo mode
- Hỗ trợ nhiều output LED, playback, streaming, recording

## Sơ đồ chân kết nối (Pinout)
| Chức năng         | GPIO |
|-------------------|------|
| LED OUT 1         | 5    |
| LED OUT 2         | 18   |
| LED OUT 3         | 19   |
| LED OUT 4         | 21   |
| LCD SDA           | 21   |
| LCD SCL           | 22   |
| Encoder A         | 32   |
| Encoder B         | 33   |
| Encoder Button    | 25   |
| Status LED        | 2    |
| W5500 CS          | 15   |
| SD Card CS        | 4    |

## Cài đặt nhanh
```sh
git clone https://github.com/truongcongdinh97/ArtNet_LED-Controller.git
# Mở bằng PlatformIO, build & upload lên ESP32
```

## Hướng dẫn sử dụng
- Khi ESP32 chưa cấu hình WiFi, thiết bị sẽ phát WiFi ở chế độ AP với tên: **LED_Controller-Setup**
  - Địa chỉ truy cập WebUI: **http://192.168.4.1**
- Truy cập WebUI để cấu hình WiFi, IP, số lượng LED, OTA firmware
- Chọn mode (Streaming, Recording, Playback) bằng encoder hoặc WebUI
- Quan sát status LED để biết trạng thái hoạt động:
  - Xanh dương: Streaming
  - Đỏ nhấp nháy: Recording
  - Xanh lá: Playback

## API cấu hình WebUI
- `GET /api/config` — Lấy cấu hình hiện tại
- `POST /api/config` — Gửi cấu hình mới (WiFi, IP, outputs, ledsPerOutput)
- `POST /api/ota` — Cập nhật firmware OTA

## Đóng góp & License
MIT License. Đóng góp vui lòng tạo pull request hoặc issue!

## Liên hệ
- Tác giả: Truong Cong Dinh
- Email: truongcongdinh97@gmail.com
- [Github](https://github.com/truongcongdinh97/ArtNet_LED-Controller)

---

> Dự án này sử dụng PlatformIO, FastLED, ESPAsyncWebServer, SD, W5500, LCD I2C, rotary encoder và các thư viện open source khác.

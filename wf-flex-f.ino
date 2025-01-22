#include <WiFi.h>
#include <WiFiUdp.h>

// Các chân kết nối flex sensor (GPIO của ESP32)
const int flex1 = 33;
const int flex2 = 32;
const int flex3 = 34;
const int flex4 = 35;
const int flex5 = 39;

// Hệ số điều chỉnh cho flex3 (chân 34)
float flex34_scale_factor = 6.0;

// Cài đặt Wi-Fi
const char* ssid = "ESP32_T";       // Tên mạng Wi-Fi của bàn tay robot
const char* password = "12345678";   // Mật khẩu mạng Wi-Fi

WiFiUDP udp;
const int udpPort = 1234;            // Cổng UDP server

void setup() {
  Serial.begin(115200); // Giao tiếp debug

  // Kết nối Wi-Fi với Access Point của bàn tay robot
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Khởi động UDP server
  udp.begin(udpPort);
  Serial.println("UDP Server started");

  // Cấu hình các chân analog
  pinMode(flex1, INPUT);
  pinMode(flex2, INPUT);
  pinMode(flex3, INPUT);
  pinMode(flex4, INPUT);
  pinMode(flex5, INPUT);
}

void loop() {
  // Đọc giá trị cảm biến
  int val1 = analogRead(flex1);
  int val2 = analogRead(flex2);
  int val3 = analogRead(flex3);
  int val4 = analogRead(flex4);
  int val5 = analogRead(flex5);

  // Áp dụng hệ số điều chỉnh cho flex3 (chân 34)
  val3 = val3 * flex34_scale_factor;

  // Giới hạn giá trị cho flex3 (chân 34)
  val3 = constrain(val3, 0, 4095);

  // Chuẩn bị chuỗi dữ liệu
  String data = String(val1) + "," + String(val2) + "," + String(val3) + "," + String(val4) + "," + String(val5);

  // Gửi dữ liệu qua UDP
  udp.beginPacket(IPAddress(192, 168, 4, 1), udpPort); // Địa chỉ IP của bàn tay robot
  udp.print(data);
  udp.endPacket();
  Serial.println(data); // Debug dữ liệu gửi
  delay(100); // Gửi mỗi 100ms
}

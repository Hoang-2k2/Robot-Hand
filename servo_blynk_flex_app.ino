#define BLYNK_TEMPLATE_ID "TMPL62L8_8Cwa"
#define BLYNK_TEMPLATE_NAME "Robot Hand"
#define BLYNK_AUTH_TOKEN "_mJgeC6_YxnwyQ01qhqSXoAWu6P9ryIQ"

#include <WiFi.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

// Thông tin Wi-Fi
const char* ssidAP = "ESP32_T";     // Tên Wi-Fi (AP)
const char* passwordAP = "12345678"; // Mật khẩu Wi-Fi (AP)

char ssid[] = "Steins Gate";  // Tên Wi-Fi
char pass[] = "12345678";     // Mật khẩu Wi-Fi

// Token Blynk
char auth[] = BLYNK_AUTH_TOKEN;

// UDP Client
WiFiUDP udp;
const int udpPort = 1234;            // Cổng UDP client
char incomingPacket[255];            // Bộ đệm dữ liệu nhận

// Servo và các chân kết nối
Servo servos[5];
int servoPins[5] = {15, 16, 17, 18, 19}; // GPIO kết nối các servo

// Chế độ điều khiển (0: Găng tay, 1: App Blynk)
int controlMode = 0; // Mặc định dùng găng tay

// Xử lý dữ liệu từ app Blynk
BLYNK_WRITE(V0) {
  controlMode = param.asInt(); // Cập nhật chế độ điều khiển
  Serial.print("Chế độ điều khiển: ");
  Serial.println(controlMode == 0 ? "Găng tay" : "App Blynk");
}

BLYNK_WRITE(V1) {
  if (controlMode == 1) {
    int angle = param.asInt();
    servos[0].write(angle);
  }
}

BLYNK_WRITE(V2) {
  if (controlMode == 1) {
    int angle = param.asInt();
    servos[1].write(angle);
  }
}

BLYNK_WRITE(V3) {
  if (controlMode == 1) {
    int angle = param.asInt();
    servos[2].write(angle);
  }
}

BLYNK_WRITE(V4) {
  if (controlMode == 1) {
    int angle = param.asInt();
    servos[3].write(angle);
  }
}

BLYNK_WRITE(V5) {
  if (controlMode == 1) {
    int angle = param.asInt();
    servos[4].write(angle);
  }
}

void setup() {
  // Khởi động Serial Monitor
  Serial.begin(115200);

  // Kết nối Blynk
  Blynk.begin(auth, ssid, pass);

  // Khởi tạo Wi-Fi ở chế độ Access Point (AP)
  WiFi.softAP(ssidAP, passwordAP);
  Serial.println("Access Point Started");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Khởi động UDP client
  udp.begin(udpPort);
  Serial.println("UDP Client started");

  // Gắn chân cho servo
  for (int i = 0; i < 5; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(90); // Đặt góc ban đầu là 90°
  }

  Serial.println("Hệ thống đã sẵn sàng!");
}

void loop() {
  Blynk.run();

  if (controlMode == 0) {
    // Kiểm tra xem có dữ liệu UDP nhận được không
    int packetSize = udp.parsePacket();
    if (packetSize) {
      int len = udp.read(incomingPacket, 255);
      if (len > 0) {
        incomingPacket[len] = 0; // Kết thúc chuỗi
      }
      Serial.print("Received packet: ");
      Serial.println(incomingPacket);

      // Phân tích dữ liệu
      String data = String(incomingPacket);
      int values[5];
      int index = 0;
      int startIndex = 0;
      for (int i = 0; i < data.length(); i++) {
        if (data[i] == ',' || i == data.length() - 1) {
          String value = data.substring(startIndex, i + (i == data.length() - 1 ? 1 : 0));
          values[index++] = value.toInt();
          startIndex = i + 1;
        }
      }

      // Chuyển đổi giá trị analog (0-4095) thành góc servo (0-180)
      for (int i = 0; i < 5; i++) {
        int angle = map(values[i], 0, 4095, 0, 180);
        servos[i].write(angle);
      }

      // Debug góc servo
      Serial.print("Servo Angles: ");
      for (int i = 0; i < 5; i++) {
        Serial.print(map(values[i], 0, 4095, 0, 180));
        if (i < 4) Serial.print(", ");
      }
      Serial.println();
    }

    delay(10); // Chờ vòng lặp
  }
}

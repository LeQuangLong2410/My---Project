#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ====== Cấu hình DHT22 ======
#define DHTPIN 21
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ====== Cấu hình WiFi ======
const char* ssid = "Wokwi-GUEST";   // WiFi mặc định của Wokwi
const char* password = "";

// ====== Cấu hình MQTT ======
const char* mqtt_server = "broker.hivemq.com";  // Broker công cộng
const int mqtt_port = 1883;
const char* mqtt_client_id = "esp32_dht22_sync_client";

// === Các topic dữ liệu cảm biến ===
const char* mqtt_topic_temp = "esp32/dht22/temperature";
const char* mqtt_topic_humi = "esp32/dht22/humidity";
const char* mqtt_topic_data  = "esp32/dht22/data"; // dữ liệu tổng hợp JSON
const char* mqtt_topic_cmd   = "esp32/dht22/command"; // điều khiển LED
const char* mqtt_topic_state = "esp32/dht22/state"; // phản hồi LED

// === Các topic điều khiển từ app ===
const char* mqtt_topic_temp_ctrl = "esp32/dht22/control/temperature";
const char* mqtt_topic_humi_ctrl = "esp32/dht22/control/humidity";

WiFiClient espClient;
PubSubClient client(espClient);

const int LED_PIN = 2;
unsigned long lastPublish = 0;
const unsigned long PUBLISH_INTERVAL = 5000; // 5 giây

// Biến lưu giá trị điều chỉnh từ app
float setTemp = 25.0;
float setHumi = 60.0;

// ====== Kết nối WiFi ======
void setup_wifi() {
  Serial.println();
  Serial.print("🔌 Đang kết nối WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi đã kết nối!");
  Serial.print("📡 IP: ");
  Serial.println(WiFi.localIP());
}

// ====== Callback MQTT ======
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();

  Serial.print("📩 Nhận từ ");
  Serial.print(topic);
  Serial.print(" → ");
  Serial.println(msg);

  // === Điều khiển LED ===
  if (String(topic) == mqtt_topic_cmd) {
    msg.toUpperCase();
    if (msg == "ON") {
      digitalWrite(LED_PIN, HIGH);
      client.publish(mqtt_topic_state, "ON");
      Serial.println("💡 LED → BẬT");
    } else if (msg == "OFF") {
      digitalWrite(LED_PIN, LOW);
      client.publish(mqtt_topic_state, "OFF");
      Serial.println("💡 LED → TẮT");
    }
  }

  // === Nhận giá trị từ Slider Nhiệt độ ===
  else if (String(topic) == mqtt_topic_temp_ctrl) {
    setTemp = msg.toFloat();
    Serial.printf("🌡 Đặt lại ngưỡng nhiệt độ: %.2f °C\n", setTemp);
  }

  // === Nhận giá trị từ Slider Độ ẩm ===
  else if (String(topic) == mqtt_topic_humi_ctrl) {
    setHumi = msg.toFloat();
    Serial.printf("💧 Đặt lại ngưỡng độ ẩm: %.2f %%\n", setHumi);
  }
}

// ====== Kết nối lại MQTT ======
void reconnect() {
  while (!client.connected()) {
    Serial.print("⏳ Kết nối MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("✅ Thành công!");
      client.subscribe(mqtt_topic_cmd);
      client.subscribe(mqtt_topic_temp_ctrl);
      client.subscribe(mqtt_topic_humi_ctrl);
      client.publish(mqtt_topic_state, digitalRead(LED_PIN) ? "ON" : "OFF");
    } else {
      Serial.print("❌ Lỗi: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

// ====== Gửi dữ liệu JSON và từng giá trị riêng ======
void publishData(float t, float h) {
  // Gửi riêng từng giá trị để HiveMQ hiển thị rõ
  char tempStr[8];
  char humiStr[8];
  dtostrf(t, 1, 2, tempStr);
  dtostrf(h, 1, 2, humiStr);
  client.publish(mqtt_topic_temp, tempStr);
  client.publish(mqtt_topic_humi, humiStr);

  // Gửi dữ liệu tổng hợp JSON
  StaticJsonDocument<200> doc;
  doc["temperature"] = t;
  doc["humidity"] = h;
  doc["set_temperature"] = setTemp;
  doc["set_humidity"] = setHumi;
  doc["led_state"] = digitalRead(LED_PIN) ? "ON" : "OFF";

  char jsonBuffer[200];
  size_t len = serializeJsonPretty(doc, jsonBuffer);
  client.publish(mqtt_topic_data, jsonBuffer, len);

  // In ra Serial
  Serial.println("📤 Cập nhật dữ liệu:");
  Serial.printf("   🌡 Nhiệt độ thực tế: %.2f °C | 💧 Độ ẩm thực tế: %.2f %%\n", t, h);
  Serial.printf("   🎚 Ngưỡng đặt: %.2f °C | %.2f %% | 💡 LED: %s\n\n",
                setTemp, setHumi, digitalRead(LED_PIN) ? "ON" : "OFF");
}

// ====== Khởi tạo ======
void setup() {
  Serial.begin(115200);
  Serial.println("🚀 ESP32 DHT22 + MQTT + Slider Sync");
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ====== Vòng lặp chính ======
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi mất kết nối, thử lại...");
    WiFi.reconnect();
    delay(1000);
    return;
  }

  if (!client.connected()) reconnect();
  client.loop();

  unsigned long now = millis();
  if (now - lastPublish > PUBLISH_INTERVAL) {
    lastPublish = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("⚠️ Lỗi đọc DHT22!");
      return;
    }

    publishData(t, h);
  }
}

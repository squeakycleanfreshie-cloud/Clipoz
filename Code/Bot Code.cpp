#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>

const char* WIFI_SSID     = "OPTUS_8E8E13";
const char* WIFI_PASSWORD = "kanzudepth68209";
const char* BOT_TOKEN     = "8527784768:AAFEATv28sBIjna2UbHRBVBpoOtUSur-h3M";
const char* CHAT_ID       = "8723339683";

#define WAKE_PIN GPIO_NUM_33

RTC_DATA_ATTR unsigned long lastNotifyTime = 0;
#define NOTIFY_COOLDOWN 30

void setup() {
  Serial.begin(115200)

  esp_sleep_wakeup_cause_t wakeReason = esp_sleep_get_wakeup_cause();

  if (wakeReason == ESP_SLEEP_WAKEUP_EXTO) {
    unsigned long now = millis();
    if (now - lastNotifyTime > (NOTIFY_COOLDOWN * 1000)) {
      connectWiFi();
      if (WiFi.status() == (WL_CONNECTED * 1000 )) {
        sendTelegram("Your freinds Keychain is on the move!")
        lastNotifyTime = millis();
      }
      Wifi.disconnect(true);
    }
  } else {
    setupMPU6050();
  }
  goToSleep();
}

void loop() {
  // put your main code here, to run repeatedly:
void connectWifi() {
  Wifi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(300);
  }
}
void sendTelegram(String message) {
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(BOT_TOKEN) + "/sendMessage";
  String payload = "{\"chat_id\":\"" + String(CHAT_ID) + "\",\"text\":\"" + message + "\"}";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.POST(payload);
  http.end();
}

void setupMPU6050() {
  Wire.begin(21, 22);
  delay(100);
  writeMPU(0x6B, 0x00);
  delay(100);
  writeMPU(0x1C, 0x00);
  writeMPU(0x1F, 10);
  writeMPU(0x20, 40);
  writeMPU(0x38, 0x40);
  writeMPU(0x37, 0x20);
}

void writeMPU(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(0x68);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void goToSleep() {
  esp_sleep_enable_ext0_wakeup(WAKE_PIN, 1);
  esp_deep_sleep_start();
}

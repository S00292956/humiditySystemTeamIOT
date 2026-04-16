#include <WiFi.h> // Use <ESP8266WiFi.h> if using ESP8266

const char* ssid = "IOT-MPSK";
const char* password = "pllcdzlq";

const char* host = "api.pushingbox.com";
const char* deviceID = "vF8F263F5DE68AE3"; // Your PushingBox DeviceID

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  WiFiClient client;
  if (client.connect(host, 80)) {
    String url = "/pushingbox?devid=" + String(deviceID);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("Email triggered!");
  }
  delay(60000); // Wait 1 minute before next trigger
}

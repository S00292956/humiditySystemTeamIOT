#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <ArduinoHttpClient.h>
#include "DHT.h"
#include "arduino_secrets.h"

// --- Hardware Setup ---
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char server[] = "script.google.com";
const int port = 443;
String urlPath = "/macros/s/AKfycbyAS94dO9l1y_GCBsv0GvId-xI8vyWUXaIQaTE-PfMfDo4f6Pj8jD1ielzHay6C2pzQrw/exec"; 

WiFiSSLClient wifi;
HttpClient client(wifi, server, port);

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  // Wi-Fi Connection
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

// --- Modified POST Function ---
void postToGoogleSheets(float hum, float temp) {
  // Update JSON keys to match your Google Apps Script expectations
  String json = "{\"temperature\":" + String(temp) + ", \"humidity\":" + String(hum) + "}";

  client.beginRequest();
  client.post(urlPath);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", json.length());
  client.beginBody();
  client.print(json);
  client.endRequest();

  // Consume response to free up client
  client.responseStatusCode();
  client.responseBody();
  client.stop();
}

void loop() {
  delay(2000); // DHT11 needs time between reads

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Sending to Sheets: ");
  Serial.print(t); Serial.print("°C, ");
  Serial.print(h); Serial.println("%");

  postToGoogleSheets(h, t);
  
  delay(60000); // Send once per minute to avoid hitting Google script quotas
}
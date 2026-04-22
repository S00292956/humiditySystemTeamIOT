#include <WiFiS3.h>
#include <DHT.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <ArduinoHttpClient.h>

// --- WiFi Settings ---
const char ssid[] = "IOT-MPSK";
const char password[] = "tjrgtmsj";

// --- PushingBox IDs ---
const char host[] = "api.pushingbox.com";
// Use the devid for the email/scenario trigger
const char emailDevid[] = "v963896FDE673C9F"; 
// Use your Google Sheets integration devid here
const char sheetDevid[] = "vF8F263F5DE68AE3"; 

WiFiClient wifi;
HttpClient client(wifi, host, 80);

// --- Hardware ---
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
rgb_lcd lcd;
const int buttonPin = 3;

bool emailSent = false;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();
  pinMode(buttonPin, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) return;

  // 1. Send data to Google Sheets every loop (or add a timer)
  sendToGoogleSheets(h, t);

  // 2. Logic for LCD and Email
  if (digitalRead(buttonPin) == LOW) {
    showReadings(h, t);
  } else {
    bool highRisk = MoldRisk(h, t);
    if (highRisk && !emailSent) {
      sendEmailAlert();
      emailSent = true;
    } else if (!highRisk) {
      emailSent = false;
    }
  }
  delay(5000); // Delay between sensor reads/updates
}

void sendToGoogleSheets(float h, float t) {
  // Replace the query parameters with those required by your Google Sheets script
  String path = "/pushingbox?devid=" + String(sheetDevid) + 
                "&humidity=" + String(h) + "&temp=" + String(t);
  client.get(path);
  client.responseStatusCode(); // Consume response
}

void sendEmailAlert() {
  String path = "/pushingbox?devid=" + String(emailDevid);
  client.get(path);
  client.responseStatusCode();
  Serial.println("Email Alert Sent");
}

// Keep your existing MoldRisk and showReadings functions here...
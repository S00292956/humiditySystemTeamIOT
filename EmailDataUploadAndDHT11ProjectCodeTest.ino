#include <WiFiS3.h>
#include <WiFiSSLClient.h>    // Required for Google Sheets (HTTPS)
#include <ArduinoHttpClient.h> // For Google Sheets POST
#include "DHT.h"
#include <Wire.h>
#include "rgb_lcd.h"


// --- Configuration ---
char ssid[] = "IOT-MPSK";
char pass[] = "pbscttbf";

// Google Sheets
const char server[] = "script.google.com";
const int port = 443;
String urlPath = "/macros/s/AKfycbyAS94dO9l1y_GCBsv0GvId-xI8vyWUXaIQaTE-PfMfDo4f6Pj8jD1ielzHay6C2pzQrw/exec"; 

// PushingBox
const char* host = "api.pushingbox.com";
String deviceID = "v9474515F6B6AEE6";

// --- Hardware ---
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
rgb_lcd lcd;
int colorR = 0;
int colorG = 255;
int colorB = 0;
const int buttonPin = 3;

// --- Clients ---
WiFiSSLClient sslClient;           // For Google Sheets
HttpClient httpSheet(sslClient, server, port);
WiFiClient standardClient;         // For PushingBox

bool emailSent = false;

void setup() {
  Serial.begin(9600);
  
  // LCD & Sensors
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT);
  dht.begin();

  // WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Sensor error");
    return;
  }

  // --- Logic Selection ---
  if (digitalRead(buttonPin) == LOW) {
    displayReadings(h, t);
  } else {
    bool highRisk = MoldRisk(h, t);
    if (highRisk && !emailSent) {
      sendEmail();
      emailSent = true;
    }
    if (!highRisk) emailSent = false;
  }

  // --- Post to Sheets (every cycle) ---
  postToGoogleSheets(h, t);

  delay(5000); // Wait 5 seconds between full loops
}

void postToGoogleSheets(float hum, float temp) {
  String json = "{\"temperature\":" + String(temp) + ", \"humidity\":" + String(hum) + "}";
  
  httpSheet.beginRequest();
  httpSheet.post(urlPath);
  httpSheet.sendHeader("Content-Type", "application/json");
  httpSheet.sendHeader("Content-Length", json.length());
  httpSheet.beginBody();
  httpSheet.print(json);
  httpSheet.endRequest();
  
  httpSheet.responseStatusCode();
  httpSheet.stop();
}

void sendEmail() {
  if (standardClient.connect(host, 80)) {
    standardClient.print(String("GET /pushingbox?devid=") + deviceID + " HTTP/1.1\r\n" +
                         "Host: " + host + "\r\n" +
                         "Connection: close\r\n\r\n");
    Serial.println("Email sent!");
  }
  standardClient.stop();
}

void displayReadings(float h, float t) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity: "); lcd.print(h); lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Temp: "); lcd.print(t); lcd.print("C");
}

bool MoldRisk(float humidity, float temp) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mold Risk:");
  bool highHumidity = humidity >= 60;
  bool highTemp = (temp >= 25 && temp <= 30);

  if (highHumidity || highTemp) {
    lcd.setRGB(255, 0, 0);
    lcd.print("High");
    return true;
  } else {
    lcd.setRGB(0, 100, 150);
    lcd.print("Normal");
    return false;
  }
}
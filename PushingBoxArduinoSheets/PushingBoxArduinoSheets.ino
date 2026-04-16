/*
  Arduino UNO R4 WiFi version of the Arduino Yún + PushingBox sketch

  Requires:
  - WiFiS3 (comes with the UNO R4 WiFi core)
  - ArduinoHttpClient library
*/

#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

#include "arduino_secrets.h"  // defines SECRET_SSID and SECRET_PASS

/////////////////
// MODIFY HERE //
/////////////////

// Add your PushingBox Scenario DeviceID here:
const char devid[] = "v963896FDE673C9F";

/////////////////
//   End       //
/////////////////

const char serverName[] = "api.pushingbox.com";
const int serverPort = 80; // PushingBox HTTP API

bool DEBUG = true;

// Network + HTTP clients
WiFiClient wifi;
HttpClient client(wifi, serverName, serverPort);

void connectWiFi()
{
  if (DEBUG) Serial.print("Connecting to WiFi SSID: ");
  if (DEBUG) Serial.println(SECRET_SSID);

  int status = WiFi.begin(SECRET_SSID, SECRET_PASS);

  // Wait for connection
  while (status != WL_CONNECTED) {
    delay(500);
    if (DEBUG) Serial.print(".");
    status = WiFi.status();
  }

  if (DEBUG) {
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  while (!Serial) { /* wait for Serial */ }

  connectWiFi();

  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (DEBUG) Serial.println("WiFi disconnected. Reconnecting...");
    connectWiFi();
  }

  int sensorValue = analogRead(A0);

  // Build the URL path (not full URL) for ArduinoHttpClient
  String path = "/pushingbox?devid=" + String(devid)
              + "&IDtag=100&TimeStamp=50&TempC=" + String(sensorValue);

  if (DEBUG) {
    Serial.print("GET ");
    Serial.println(path);
  }

  // Send request
  client.get(path);

  // Read status and response (optional but useful for debugging)
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  if (DEBUG) {
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
  }

  Serial.print("Uploaded temp value: ");
  Serial.println(sensorValue);

  delay(5000);
}}

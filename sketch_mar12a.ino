/*
  Arduino Uno R4 WiFi - Send sensor value to PushingBox
*/

#include <WiFiS3.h>

// MODIFY HERE
char ssid[] = "IOT-MPSK";
char pass[] = "qczqcdsz";
char devid[] = "v9474515F6B6AEE6";
// END MODIFY

const int buttonPin = 3;

char serverName[] = "api.pushingbox.com";

WiFiClient client;

void setup() {

  Serial.begin(9600);
  while (!Serial);

  pinMode(13, OUTPUT);

  Serial.println("Connecting to WiFi...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(2000);
    Serial.println("Retrying WiFi...");
  }

  Serial.println("Connected to WiFi");
  digitalWrite(13, HIGH);
}

void loop() {

if (buttonState == LOW)
  {

  int sensorValue = analogRead(A0);

  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  if (client.connect(serverName, 80)) {

    String url = "/pushingbox?devid=" + String(devid) +
                 "&IDtag=100&TimeStamp=50&TempC=" + String(sensorValue);

    Serial.println("Sending request...");

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + serverName + "\r\n" +
                 "Connection: close\r\n\r\n");

  } 
  else {
    Serial.println("Connection failed");
  }
  }
  else{
    serial.println("Button Not Pressed");
  }
  delay(5000);
}
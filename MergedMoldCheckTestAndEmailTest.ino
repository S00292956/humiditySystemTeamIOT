#include <WiFiS3.h>
#include "DHT.h"
#include <Wire.h>
#include "rgb_lcd.h"

// -------- WIFI --------
char ssid[] = "IOT-MPSK";
char password[] = "pbscttbf";

const char* host = "api.pushingbox.com";
String deviceID = "vF8F263F5DE68AE3";

WiFiClient client;

// -------- DHT --------
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// -------- LCD --------
rgb_lcd lcd;
int colorR = 0;
int colorG = 255;
int colorB = 0;

// -------- BUTTON --------
const int buttonPin = 3;
int buttonState = 0;

// -------- CONTROL --------
bool emailSent = false;

void setup() {
  Serial.begin(9600);

  // LCD
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);

  // Button
  pinMode(buttonPin, INPUT);

  // Sensor
  dht.begin();

  // WiFi
  Serial.print("Connecting to WiFi");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(3000);
  }
  Serial.println("\nConnected!");
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Sensor error");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println("%");

  Serial.print("Temp: ");
  Serial.print(t);
  Serial.println("C");

  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    // Show readings
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");
    lcd.print(h);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print((char)223);
    lcd.print("C");

  } else {
    // Show mold risk
    bool highRisk = MoldRisk(h, t);

    // SEND EMAIL ONLY ON HIGH RISK
    if (highRisk && !emailSent) {
      sendEmail();
      emailSent = true;
    }

    // Reset if conditions go back to normal
    if (!highRisk) {
      emailSent = false;
    }
  }

  delay(2000);
}

// -------- MOLD LOGIC --------
bool MoldRisk(float humidity, float temp) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mold Risk:");

  //  FIXED LOGIC
  bool highHumidity = humidity >= 60;
  bool highTemp = (temp >= 25 && temp <= 30);

  if (highHumidity || highTemp) {
    lcd.setRGB(255, 0, 0);
    lcd.print("High");

    lcd.setCursor(0, 1);
    if (highHumidity && highTemp) {
      lcd.print("T & H High");
    } else if (highHumidity) {
      lcd.print("Humidity High");
    } else {
      lcd.print("Temp High");
    }

    return true;
  }

  else if ((humidity >= 25 && humidity <= 30) && (temp >= 25 && temp <= 30)) {
    lcd.setRGB(0, 255, 0);
    lcd.print("Low");

    lcd.setCursor(0, 1);
    lcd.print("Perfect T+H");

    return false;
  }

  else {
    lcd.setRGB(0, 100, 150);
    lcd.print("Normal");
    return false;
  }
}

// -------- EMAIL FUNCTION --------
void sendEmail() {
  Serial.println("Sending email alert...");

  if (client.connect(host, 80)) {
    String url = "/pushingbox?devid=" + deviceID;

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("Email sent!");
  } else {
    Serial.println("Email failed");
  }

  client.stop();
}
/*
  SendAnEmail (WiFi S3 Version)
  Uses Temboo with WiFiS3 to send an email.
  Target Board: Arduino UNO R4 WiFi
*/

#include <SPI.h>
#include <WiFiS3.h> // Updated for UNO R4 WiFi
#include <Temboo.h>

// --- WIFI CONFIGURATION ---
char ssid[] = "IOT-MPSK";
char pass[] = "Password Here";
int status = WL_IDLE_STATUS;     

// --- TEMBOO CONFIGURATION ---
#define TEMBOO_ACCOUNT "CA"
#define TEMBOO_APP_KEY_NAME "myFirstApp"
#define TEMBOO_APP_KEY "pQqv8URtBnDcKrJIamnRK9i2cvhytRvn"

WiFiClient client;

// --- GMAIL CONFIGURATION ---
// Note: Use a Google 'App Password' here if standard login fails
const String GMAIL_USER_NAME = "arduinoreceiveremailapi@gmail.com";
const String GMAIL_PASSWORD = "ArduinoReceiver123"; 
const String TO_EMAIL_ADDRESS = "YourEmailHere";

boolean success = false; 

void setup() {
  Serial.begin(9600);
  
  // Wait for Serial to initialize
  unsigned long startMillis = millis();
  while (!Serial && millis() - startMillis < 5000);

  // Attempt to connect to WiFi network
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    // Wait 10 seconds for connection
    delay(10000);
  }

  Serial.println("Connected to WiFi!");
  printWiFiStatus();
}

void loop() {
  // We only want to send the email once in this example
  if (!success) {
    Serial.println("Running SendAnEmail via Temboo...");

    // Pass the WiFiClient to the TembooChoreo object
    TembooChoreo SendEmailChoreo(client);

    SendEmailChoreo.begin();

    // Set Temboo account credentials
    SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);

    // Identify the Choreo to run
    SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");

    // Set Choreo inputs
    SendEmailChoreo.addInput("Username", GMAIL_USER_NAME);
    SendEmailChoreo.addInput("Password", GMAIL_PASSWORD);
    SendEmailChoreo.addInput("ToAddress", TO_EMAIL_ADDRESS);
    SendEmailChoreo.addInput("Subject", "ALERT: Greenhouse Temperature");
    SendEmailChoreo.addInput("MessageBody", "Hey! The greenhouse is too cold!");

    // Run the Choreo
    unsigned int returnCode = SendEmailChoreo.run();

    if (returnCode == 0) {
      Serial.println("Success! Email sent!");
      success = true;
    } else {
      // If there's an error, print the response from Temboo
      while (SendEmailChoreo.available()) {
        char c = SendEmailChoreo.read();
        Serial.print(c);
      }
      Serial.println();
    }
    SendEmailChoreo.close();
  }
  
  // Wait 60 seconds before looping (though 'success' flag prevents re-sending)
  delay(60000); 
}

void printWiFiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}
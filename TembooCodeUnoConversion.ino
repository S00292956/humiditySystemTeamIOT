#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "yourSSID"
#define WIFI_PASSWORD "yourPASSWORD"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "your@gmail.com"
#define AUTHOR_PASSWORD "your_app_password"

#define RECIPIENT_EMAIL "receiver@gmail.com"

SMTPSession smtp;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  SMTP_Message message;

  message.sender.name = "Arduino";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ALERT: Greenhouse Temperature";
  message.addRecipient("User", RECIPIENT_EMAIL);

  message.text.content = "Hey! The greenhouse is too cold!";

  Session_Config config;
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;

  if (!smtp.connect(&config)) {
    Serial.println("Connection failed");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending Email");
  } else {
    Serial.println("Email sent!");
  }
}

void loop() {}
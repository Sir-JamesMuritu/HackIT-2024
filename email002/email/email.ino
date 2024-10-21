#include <Firebase.h>
#include <Arduino.h>
#include <time.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465


#include "secrets.h"


// Define your SMTP email credentials


// Define the soil moisture sensor pin
const int soilMoisturePin = A0;  // Adjust according to your wiring
int soilMoistureValue;
String DeviceId="";


Firebase fb(REFERENCE_URL, AUTH_TOKEN); 

SMTPSession smtp;
void smtpCallback(SMTP_Status status);

void setup() {
  Serial.begin(9600);
  
  // Wi-Fi configuration and connection
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  String macAddress = WiFi.macAddress();
  DeviceId = "Device_"+String(macAddress);

  Serial.println();
  Serial.println("Connecting to: " + String(WIFI_SSID));
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("-");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Initialize NTP to get the current time
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "EAT-3", 1);  // East Africa Time (adjust according to your timezone)
  tzset();

  // Initialize SMTP settings
  MailClient.networkReconnect(true);
  smtp.debug(1);  // Enable SMTP debugging
  smtp.callback(smtpCallback);  // Set callback for SMTP status
  
  // // Firebase setup: Setting and Pushing data
  // fb.setString("Example/myString", "Hello World!");
  // fb.setInt("Example/myInt", 123);
  // fb.setFloat("Example/myFloat", 45.67);
  // fb.setBool("Example/myBool", true);

  // fb.pushString("Push", "Foo-Bar");
  // fb.pushInt("Push", 890);
  // fb.pushFloat("Push", 12.34);
  // fb.pushBool("Push", false);
  
  // Initialize the soil moisture sensor pin
  pinMode(soilMoisturePin, 0);
}

void loop() {
  // Read soil moisture value
  soilMoistureValue = analogRead(soilMoisturePin);

  Serial.print("Soil Moisture Level: ");
  Serial.println(soilMoistureValue);

  // Save the data to Firebase
  saveToCloudDb();

  // Send an email alert if the soil moisture value is beyond the threshold
  if (soilMoistureValue >= 500) {  // Adjust threshold based on testing
    sendEmailAlert();
  }

  delay(1000);  // Wait for 1 minute before the next check
}

void saveToCloudDb() {
   // Get current time
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  char timestamp[30];
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);  // Format as HH:MM:SS

  // Format the date as YYYY-MM-DD
  char date[30];
  strftime(date, sizeof(date), "%Y-%m-%d", timeinfo);  // Format as YYYY-MM-DD

  String path = "/UrineDetectorSystem/"+DeviceId+"/Readings/" + String(date) + "/" + String(timestamp);

  // Save data to Firebase using push
  fb.setInt(path +"/moisturelevel", soilMoistureValue);
}

void sendEmailAlert() {
  // Set up the SMTP session and email details
  Session_Config config;
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;

  SMTP_Message message;
  message.sender.name = F("Urine Detector System");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("Urine Detector Alert");
  message.addRecipient(F("Recipient"), RECIPIENT_EMAIL);

  // Prepare the email content with soil moisture level
  String textMsg = "Alert! The bed is wet, need attention!! \n With level: " + String(soilMoistureValue);
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // Connect and send the email
  if (!smtp.connect(&config)) {
    Serial.println("SMTP connection failed");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Email sending failed");
  }
}

void smtpCallback(SMTP_Status status) {
  // Handle SMTP callback status
  Serial.println(status.info());

  if (status.success()) {
    Serial.println("Email sent successfully!");
  }
}

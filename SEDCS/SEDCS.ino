#include <ArduinoJson.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>


#define RST_PIN D1 
#define SS_PIN D2
#define GREEN_LED D3
#define RED_LED D4

MFRC522 mfrc522(SS_PIN, RST_PIN);  
char jsonOutput[128];
String deviceName = "D01";  // Device name defined in the code
String cardData = "";
const char* ssid = "JACKSON";
const char* password = "mwaya.123";

ESP8266WebServer server(80);

void setup() { 

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..."); 

    digitalWrite(RED_LED, LOW); // Turn on red LED
    delay(500);
    digitalWrite(RED_LED, HIGH); // Turn off red LED
    delay(500); 
    digitalWrite(RED_LED, LOW); 
  }
  
  digitalWrite(GREEN_LED, LOW); // Turn on green LED
  delay(500);
  digitalWrite(GREEN_LED, HIGH); // Turn off green LED
  delay(500);
  digitalWrite(GREEN_LED, LOW); // Turn on green LED
  delay(500);
  digitalWrite(GREEN_LED, HIGH); // Turn off green LED
  delay(500);
  digitalWrite(GREEN_LED, LOW); 

   Serial.println("Connected to WiFi");  



  // Register the endpoint for receiving session ID
  server.on("/GetData", handleGetData);

  server.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      String cardID = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        cardID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        cardID += String(mfrc522.uid.uidByte[i], HEX);
      }

      cardData = cardID; 
      Serial.print("Card ID: "); 
      Serial.println(cardID); 

      digitalWrite(GREEN_LED, LOW); // Turn on green LED
      delay(500);
      digitalWrite(GREEN_LED, HIGH); // Turn off green LED
      delay(500);
      digitalWrite(GREEN_LED, LOW); // Turn on green LED
      delay(500);
      digitalWrite(GREEN_LED, HIGH); // Turn off green LED
      delay(500);
      digitalWrite(GREEN_LED, LOW); 


      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }

  server.handleClient();
}



void handleGetData() {
  server.sendHeader("Access-Control-Allow-Origin", "http://192.168.43.109:3000");
  server.sendHeader("Access-Control-Allow-Methods", "POST");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.sendHeader("Content-Type", "application/json");
  server.sendHeader("Access-Control-Allow-Credentials", "true");

  if (server.method() == HTTP_OPTIONS) {
    server.send(204); // No content required for OPTIONS request
    return;
  }

  DynamicJsonDocument responseDoc (4096);
  JsonObject object = responseDoc.to<JsonObject>(); 

  object["cardData"] = cardData;
  object["deviceName"] = deviceName;

  // Convert the response JSON to a String
  String responsePayload;
  serializeJson(responseDoc, jsonOutput);

  // Send the JSON response
  server.send(200, "application/json", String(jsonOutput));  

  digitalWrite(GREEN_LED, LOW); // Turn on green LED
  delay(500);
  digitalWrite(GREEN_LED, HIGH);
  delay(500);
  digitalWrite(GREEN_LED, LOW); 
  delay(500);
  digitalWrite(GREEN_LED, HIGH); 
  delay(500);
  digitalWrite(GREEN_LED, LOW); 

  Serial.println("Card data sent to successfully.");  
}




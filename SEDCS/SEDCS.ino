#include <ArduinoJson.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

#define SS_PIN D2
#define RST_PIN D1 
#define GREEN_LED D3
#define RED_LED D4

MFRC522 mfrc522(SS_PIN, RST_PIN);
String sessionId = ""; // Session ID received from the React frontend
String deviceName = "DO1"; // Device name defined in the code

const char* ssid = "JACKSON";
const char* password = "mwaya.123";
const char* backendUrl = "http://192.168.43.109:8080/smartcardapp-api/device.php";

ESP8266WebServer server(80);

void setup() { 

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  //digitalWrite(GREEN_LED, HIGH); // Turn off green LED
  //digitalWrite(RED_LED, HIGH); // Turn off red LED 

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
  digitalWrite(GREEN_LED, LOW); // Turn on green LED

   Serial.println("Connected to WiFi");  



  // Register the endpoint for receiving session ID
  server.on("/SessionId", handleSessionId);

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

      Serial.print("Card ID: ");
      Serial.println(cardID);

      sendCardData(cardID);

      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }

  server.handleClient();
  // Other loop code
}

void sendCardData(String cardID) { 
  digitalWrite(GREEN_LED, LOW); // Turn on green LED
  delay(500);
  digitalWrite(GREEN_LED, HIGH);  // Turn off green LED  
  delay(500);
  digitalWrite(GREEN_LED, LOW);
  WiFiClient client;
  HTTPClient http;

  // Create a DynamicJsonDocument for the payload
  DynamicJsonDocument payloadJson(128); // Adjust the size as per your payload size

  // Add key-value pairs to the payload
  payloadJson["cardData"] = cardID;
  payloadJson["deviceName"] = deviceName;
  payloadJson["sessionId"] = sessionId;

  // Convert the payloadJson to a String
  String payload; 
  serializeJson(payloadJson, payload);

  // Print the payload
  Serial.println("Payload: ");
  Serial.println(payload);

  // Send POST request to the backend
  http.begin(client, backendUrl); 
  http.addHeader("Accept", "application/json");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("sessionId", sessionId); // Pass the session ID in the request headers

  int httpResponseCode = http.POST(payload);

  delay(1000);

  Serial.println("HTTP Response Code: ");

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode); 
    if (httpResponseCode == HTTP_CODE_OK) {  
      digitalWrite(GREEN_LED, LOW); // Turn on green LED
      delay(500);
      digitalWrite(GREEN_LED, HIGH); 
      delay(500); 
      digitalWrite(GREEN_LED, LOW); 
      Serial.println("Card data sent to backend successfully.");
    } else {
      digitalWrite(RED_LED, LOW); // Turn on red LED
      delay(500);
      digitalWrite(RED_LED, HIGH); // Turn off red LED 
      delay(500); 
      digitalWrite(RED_LED, LOW); 
      Serial.print("Failed to send card data to backend. Error code: ");
      Serial.println(httpResponseCode);
    }
  } else {
    Serial.println("Error sending HTTP request.");
  }

  http.end();
}


void handleSessionId() { 
  server.sendHeader("Content-Type", "application/json");
  server.sendHeader("Access-Control-Allow-Origin", "http://192.168.43.109:3000");
  server.sendHeader("Access-Control-Allow-Methods", " POST");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
 // server.sendHeader("Access-Control-Allow-Credentials", "true");

  if (server.hasArg("plain")) {
    // Decode the JSON object
    String jsonString = server.arg("plain");
    DynamicJsonDocument jsonDoc(256); // Adjust the size as per your JSON object size

    // Deserialize the JSON object
    DeserializationError error = deserializeJson(jsonDoc, jsonString);

    // Check for parsing errors
    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    // Assign the value of the session to the sessionId variable
    sessionId = jsonDoc["sessionId"].as<String>();

    Serial.println("Received session ID: " + sessionId);
  }

  server.send(200); // Send a response indicating the successful receipt of the session ID
}



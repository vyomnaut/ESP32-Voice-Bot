#include <WiFi.h>
#include <HTTPClient.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// === Wi-Fi Credentials ===
const char* ssid = "VYOMNAUT 6251";
const char* password = ")4v45K66";

// === Deepgram API ===
const char* serverUrl = "https://api.deepgram.com/v1/listen";
const char* apiKey = "ad60407e2610db52805fcaa768977fe93a5df25b";  // Replace with yours

// === Pins ===
#define MIC_PIN 34           // MAX9814 OUT
#define BUTTON_PIN 4         // Button
#define DFPLAYER_RX 16       // Connect to DFPlayer TX
#define DFPLAYER_TX 17       // Connect to DFPlayer RX

// === Globals ===
SoftwareSerial mySerial(DFPLAYER_RX, DFPLAYER_TX); // RX, TX
DFRobotDFPlayerMini dfplayer;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);

  // Init Button and Mic
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(MIC_PIN, INPUT);

  // Wi-Fi Connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");

  // Init DFPlayer
  if (!dfplayer.begin(mySerial)) {
    Serial.println("DFPlayer init failed!");
    while (true);
  }
  Serial.println("DFPlayer Ready!");
  dfplayer.volume(25);  // Set volume (0–30)
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println("Button Pressed!");

    int micValue = analogRead(MIC_PIN);
    Serial.println("Mic Sample: " + String(micValue));

    String reply = sendToDeepgram(micValue); // Replace this with actual audio later

    if (reply != "") {
      speakReply(reply);
    } else {
      dfplayer.playMp3Folder(2);  // Play "I didn't get that" audio
    }

    delay(1000); // Debounce
  }
}

// === Send Mic Sample to API ===
String sendToDeepgram(int audioSample) {
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Token " + String(apiKey));

  // For testing, we’re just sending dummy audio sample as text
  String json = "{\"audio_sample\":" + String(audioSample) + "}";
  int httpResponseCode = http.POST(json);

  String response = "";
  if (httpResponseCode > 0) {
    response = http.getString();
    Serial.println("Server Response: " + response);
  } else {
    Serial.println("HTTP Error: " + String(httpResponseCode));
  }

  http.end();
  return response;
}

// === Simulate Reply Audio ===
void speakReply(String reply) {
  reply.toLowerCase();

  if (reply.indexOf("yes") >= 0) {
    dfplayer.playMp3Folder(1);  // Play file001.mp3
  } else if (reply.indexOf("no") >= 0) {
    dfplayer.playMp3Folder(2);  // Play file002.mp3
  } else if (reply.indexOf("hello") >= 0) {
    dfplayer.playMp3Folder(3);  // Play file003.mp3
  } else {
    dfplayer.playMp3Folder(4);  // Unknown/default response
  }
}

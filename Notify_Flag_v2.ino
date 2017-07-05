#include <ArduinoJson.h>         // Install this library from Sketch->Include Library->Libray Manager
#include <Servo.h>
#include <ESP8266WiFi.h>

const char* THIG_NAME = "SparksESP";  // Put your thing name here
const char* key = "Servo1";               // Put your key name here

const char* WIFISSID = "SSID";     // Put your WiFi SSID here
const char* PASSWORD = "PASSWORD";    // Put your WiFi password here

int servoPin = 2;                // This is the pin to which your servo is attached to

const char* host = "dweet.io";
String ans;
String response;
uint8_t bodyPosinit;
uint8_t bodyPosend;
String dweetKey ="\"";
int length = dweetKey.length();

Servo mailFlag;

void setup() {
    
    mailFlag.attach(servoPin);
    
    Serial.begin(115200);
    Serial.println("Flag Reset");
    mailFlag.write(0);

    Serial.print("Connecting to : ");
    Serial.println(WIFISSID);
    WiFi.begin(WIFISSID,PASSWORD);
    while(WiFi.status()!= WL_CONNECTED)
    {
        delay(100);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
    Serial.println(WiFi.localIP());

    ans="False";
    dweetKey += key;
    dweetKey += "\":";
}

void loop() {
    receiveDweet();
    printValue();
    Serial.println(ans);
    if(ans=="True")
    {
      Serial.println("Flag On!");
      mailFlag.write(90);
      sendDweet();
    }
}

void sendDweet()
{
    Serial.print("Setting ");
    Serial.print(key);
    Serial.print(" of ");
    Serial.print(THIG_NAME);
    Serial.println(" to FALSE.");
    
    WiFiClient client;
    const int httpPort = 80;
    Serial.print("Connecting to : ");
    Serial.println(host);
    if (!client.connect(host, httpPort)) {
      Serial.println("Connection Failed");
      return;
    }
    Serial.println("Connected Successfully!");
    client.print(String("GET /dweet/for/"));
    client.print(THIG_NAME);
    client.print(String("?"));
    client.print(key);
    client.print(String("=False HTTP/1.1\r\n Host: dweet.io \r\n Connection: close\r\n\r\n"));
    delay(10);

    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } 

    Serial.println("\nClosing Connection");
}

void receiveDweet()
{
    Serial.print("Receiving Data for ");
    Serial.println(THIG_NAME);
    WiFiClient client;
    const int httpPort = 80;
    Serial.print("Connecting to : ");
    Serial.println(host);
    if (!client.connect(host, httpPort)) {
      Serial.println("Connection Failed");
      return;
    }
    Serial.println("Connected Successfully!");
    client.print(String("GET /get/latest/dweet/for/"));
    client.print(THIG_NAME);
    client.print(String(" HTTP/1.1\r\n Host: dweet.io \r\n Connection: close\r\n\r\n"));
    delay(10);

    while(client.available()){
     response = client.readStringUntil('\r');
      Serial.print(response);
    }

    Serial.println("\nClosing Connection");
}

void printValue()
{
    bodyPosinit =4+ response.indexOf("\r\n\r\n");
    response = response.substring(bodyPosinit);
    Serial.println(response);

    bodyPosinit =10+ response.indexOf("\"content\"");
    bodyPosend = response.indexOf("}]}");
    response = response.substring(bodyPosinit,bodyPosend);
    Serial.println(response);

    if (response.indexOf(dweetKey) == -1) {
      ans = "Key not found";
    } else {
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(response);
        if (!root.success()){
          Serial.println("parseObject() failed");
          ans = "parse error.";
        } else {
          const char* val  = root[key];
          ans = String(val);
        }
     }
}


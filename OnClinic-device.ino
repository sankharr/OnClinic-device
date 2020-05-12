#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

// Set these to run example.
#define FIREBASE_HOST "onclinic-dd11a.firebaseio.com"
#define FIREBASE_AUTH "GfXQMzod9trL7XyOIic8HwVKxbowFSSTCfVwrnLO"

#define WIFI_SSID "Redhat"
#define WIFI_PASSWORD "c#FRt/qLvD"

//#define WIFI_SSID "Hexio"
//#define WIFI_PASSWORD "dnne4216"

String myString;
int vr = A0;   // variable resistor connected
int sdata = 0; // The variable resistor value will be stored in sdata.
int inPin = D2;

String docID = "ds2DMiM3BYW95mtKkpQ2xFghFEC2";
String temp;
int buttonStatus = 0;

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(vr, INPUT);
  // connect to wifi.
  // pinMode(D0, OUTPUT);
  pinMode(inPin, INPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.print("Firebase Connection status - ");
  Serial.println(Firebase.success());
  temp = docID + "/Val";
}

void loop()
{

  buttonStatus = digitalRead(inPin);
  sdata = analogRead(vr);
  myString = String(sdata);
  Serial.println(myString);

  if (buttonStatus == HIGH)
  {
    Firebase.setString(temp, myString);
  }
  
  Serial.println(Firebase.success());
  //delay(1000);
}

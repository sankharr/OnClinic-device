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

//#define  docID = ds2DMiM3BYW95mtKkpQ2xFghFEC2;
String myString;
int vr = A0; // variable resistor connected 
int sdata = 0; // The variable resistor value will be stored in sdata.
int d0 = D0;
unsigned long time1 = 0;
unsigned long time2 = 0;
unsigned long time3 = 0;
String docID = "ds2DMiM3BYW95mtKkpQ2xFghFEC2";
String temp;


 
void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(vr ,INPUT);
  // connect to wifi.
  pinMode(D0,OUTPUT);
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
 
//    Firebase.setString("Variable/Value","fahad");
}
 
void loop()
{

//Serial.println("normalCommand - Start"); 
sdata = analogRead(vr);
//Serial.println("normalCommand - End"); 
myString = String(sdata);
//Serial.println("start1");  
Serial.println(myString);
//Serial.println("firebaseCommand - Start");
 
Firebase.setString(temp,myString);
Serial.println(Firebase.success());
delay(5000);            
}

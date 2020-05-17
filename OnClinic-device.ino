#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ONE_WIRE_BUS D3  //relevant to temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Set these to run example.
#define FIREBASE_HOST "onclinic-dd11a.firebaseio.com"
#define FIREBASE_AUTH "GfXQMzod9trL7XyOIic8HwVKxbowFSSTCfVwrnLO"

#define WIFI_SSID "Redhat"
#define WIFI_PASSWORD "c#FRt/qLvD"

//#define WIFI_SSID "Hexio"
//#define WIFI_PASSWORD "dnne4216"

String myString;
String temperatureString;
int vr = A0;   // variable resistor connected
int sdata = 0; // The variable resistor value will be stored in sdata.
int inPin = D5;

String docID = "ds2DMiM3BYW95mtKkpQ2xFghFEC2";
String temp;
String temp2;
int buttonStatus = 0;

float temperatureValue = 0;

void setup()
{
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
//  delay(2000);
  display.clearDisplay();         //Welcome Screen
  display.setCursor(35,20);
  display.setTextColor(SSD1306_WHITE); 
  display.setTextSize(1);           
  display.println("Welcome To");
  display.setTextSize(2);
  display.setCursor(16,30);  
  display.println("OnClinic");
  display.display();
  delay(2000);
  
  sensors.begin();  //relevent to temperature sensor 
  
  pinMode(vr, INPUT);
  // connect to wifi.
  pinMode(inPin, INPUT);

  display.clearDisplay();         //'Connecting' message
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(25,30);
  display.setTextSize(1);               
  display.println("Connecting....");
  display.display();
  
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

  display.clearDisplay();           //'Connection Successful' message
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(33,25);             
  display.println("Connection");
  display.setCursor(32,35);
  display.println("Successful!");
  display.display();
  delay(2000);
  
  temp = docID + "/Val";
  temp2 = docID + "/temperature";
}

void loop()
{
  getTemperatureData();  
  
  buttonStatus = digitalRead(inPin);
  
//  sdata = analogRead(vr);             //<-------for variable resistor
//  myString = String(sdata);
//  Serial.println(myString);

  temperatureString = String(temperatureValue);
   display.setTextSize(1);
   
  if (buttonStatus == HIGH)
  {
//    Firebase.setString(temp, myString);   //<------------for variable resistor
    
    Firebase.setString(temp2, temperatureString);
    display.println("");        
    display.println("Updated the database!");
    display.display();
        
    delay(1000);
  }
//  Serial.println(buttonStatus);
//  Serial.println(Firebase.success());
//  delay(1000);
}

void getTemperatureData() {
  sensors.requestTemperatures(); // Send the command to get temperatures  
  Serial.println(sensors.getTempCByIndex(0));
  temperatureValue = sensors.getTempCByIndex(0);

  display.clearDisplay();
  
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Temperature"));
  
  display.println(F(""));
  
  display.setTextSize(3);    
  display.setTextColor(SSD1306_WHITE);
//  display.setCursor(30,30);
  display.println(sensors.getTempCByIndex(0));
//  display.println("");

  display.display();
  
}

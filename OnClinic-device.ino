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

#include "MAX30105.h"           //MAX3010x library
#include "heartRate.h"          //Heart rate calculating algorithm

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.  //<-----------for heart beat sensor
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

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
String valueString;
int vr = A0;   // variable resistor connected
int sdata = 0; // The variable resistor value will be stored in sdata.
int inPin = D5;
int modeButton = D6;

String docID = "ds2DMiM3BYW95mtKkpQ2xFghFEC2";
String temp;
String temp2;
String path3;
int buttonStatus = 0;
int modeButtonStatus = 0;
bool modeSwitch = false;

int state = HIGH;
int reading;
int previous = LOW;
long timek = 0;
long debounce = 200;

float temperatureValue = 0;

static const unsigned char PROGMEM logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,              //Logo2 and Logo3 are two bmp pictures that display on the OLED if called
0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,  };

static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00  };

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
  display.setTextSize(1);
  display.setCursor(30,30);  
  display.println("HospitalCore");
  display.display();
  delay(2000);
  
  sensors.begin();  //relevent to temperature sensor 

  particleSensor.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed    //<------for heart beat sensor
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  
  pinMode(vr, INPUT);
  // connect to wifi.
  pinMode(inPin, INPUT);
  pinMode(modeButton, INPUT);

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
  path3 = docID + "/heartBeat";
}

void loop()
{
  
  reading = digitalRead(modeButton);

  if (reading == HIGH && previous == LOW && millis() - timek > debounce) {
    if (state == HIGH)
      state = LOW;
    else
      state = HIGH;

    timek = millis();    
  }

//  if (modeButtonStatus == HIGH) {
//    modeSwitch = !modeSwitch;
//    Serial.println(modeSwitch);
////    display.println("");        
////    display.println("Second button works!");
////    display.display();
//  }
  
//  sdata = analogRead(vr);             //<-------for variable resistor
//  myString = String(sdata);
//  Serial.println(myString);

  if (state == HIGH) {          //<--------- heart rate mode
    long irValue = particleSensor.getIR();    //Reading the IR value it will permit us to know if there's a finger on the sensor or not
                                               //Also detecting a heartbeat
    if(irValue > 7000){                                           //If a finger is detected
        display.clearDisplay();                                   //Clear the display
        display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);       //Draw the first bmp picture (little heart)
        display.setTextSize(2);                                   //Near it display the average BPM you can display the BPM if you want
        display.setTextColor(WHITE); 
        display.setCursor(50,0);                
        display.println("BPM");             
        display.setCursor(50,18);                
        display.println(beatAvg); 
        display.display();
        
      if (checkForBeat(irValue) == true)                        //If a heart beat is detected
      {
        display.clearDisplay();                                //Clear the display
        display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE);    //Draw the second picture (bigger heart)
        display.setTextSize(2);                                //And still displays the average BPM
        display.setTextColor(WHITE);             
        display.setCursor(50,0);                
        display.println("BPM");             
        display.setCursor(50,18);                
        display.println(beatAvg); 
        display.display();
        tone(3,1000);                                        //And tone the buzzer for a 100ms you can reduce it it will be better
        delay(100);
        noTone(3);
        buttonStatus = digitalRead(inPin);
        valueString = String(beatAvg);
        if (buttonStatus == HIGH)
        { 
          display.setTextSize(1);
          Firebase.setString(path3, valueString);
          display.println("");        
          display.println("Updated the database!");
          display.display();              
          delay(1000);
        }//Deactivate the buzzer to have the effect of a "bip"
        //We sensed a beat!
        long delta = millis() - lastBeat;                   //Measure duration between two beats
        lastBeat = millis();
    
        beatsPerMinute = 60 / (delta / 1000.0);           //Calculating the BPM
    
        if (beatsPerMinute < 255 && beatsPerMinute > 20)               //To calculate the average we strore some values (4) then do some math to calculate the average
        {
          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
          rateSpot %= RATE_SIZE; //Wrap variable
    
          //Take average of readings
          beatAvg = 0;
          for (byte x = 0 ; x < RATE_SIZE ; x++)
            beatAvg += rates[x];
          beatAvg /= RATE_SIZE;
        }
      }
    
    }
      if (irValue < 7000){       //If no finger is detected it inform the user and put the average BPM to 0 or it will be stored for the next measure
         beatAvg=0;
         display.clearDisplay();
         display.setTextSize(1);                    
         display.setTextColor(WHITE);             
         display.setCursor(30,5);                
         display.println("Please Place "); 
         display.setCursor(30,15);
         display.println("your finger ");  
         display.display();
         noTone(3);
         }
  }
  else{                         //<---------- temperature mode
    getTemperatureData();  
  
    buttonStatus = digitalRead(inPin);
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
  }

  previous = reading;
  
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

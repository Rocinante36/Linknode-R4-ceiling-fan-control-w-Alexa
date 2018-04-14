#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

// prototypes
boolean connectWifi();

//on/off callbacks 
bool FanLightOn();
bool FanLightOff();
bool FanLowOn();
bool FanLowOff();
bool FanMediumOn();
bool FanMediumOff();
bool FanHighOn();
bool FanHighOff();

// Change this before you flash
const char* ssid = "your 2.4G network";
const char* password = "your network password";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *FanLight = NULL;
Switch *FanLow = NULL;
Switch *FanMedium = NULL;
Switch *FanHigh = NULL;

// Set Relay Pins
int relayOne = 12;
int relayTwo = 13;
int relayThree = 14;
int relayFour = 16;

bool isRelayOneOn = false;
bool isRelayTwoOn = false;
bool isRelayThreeOn = false;
bool isRelayFourOn = false;

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    FanLight = new Switch("Fan Light", 80, FanLightOn, FanLightOff);
    FanLow = new Switch("Fan Low", 81, FanLowOn, FanLowOff);
    FanMedium = new Switch("Fan Medium", 82, FanMediumOn, FanMediumOff);
    FanHigh = new Switch("Fan High", 83, FanHighOn, FanHighOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*FanLight);
    upnpBroadcastResponder.addDevice(*FanLow);
    upnpBroadcastResponder.addDevice(*FanMedium);
    upnpBroadcastResponder.addDevice(*FanHigh);

    //Set relay pins to outputs
       pinMode(12,OUTPUT); 
       pinMode(13,OUTPUT);
       pinMode(14,OUTPUT);
       pinMode(16,OUTPUT);

    Serial.println("Startup: all Sockets turn off ...");
    digitalWrite(relayOne, LOW);   // sets relayOne off
    digitalWrite(relayThree, LOW);   // sets relayThree off
    digitalWrite(relayTwo, LOW);   // sets relayTwo off
    digitalWrite(relayFour, LOW);   // sets relayFour off
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      FanLight->serverLoop();
      FanLow->serverLoop();
      FanMedium->serverLoop();
      FanHigh->serverLoop();
	 }
}
 
bool FanLightOn() {
    Serial.println("Fan Light turn on ...");
    digitalWrite(relayOne, HIGH);   // sets relayOne on
    isRelayOneOn = true;
    return isRelayOneOn;
}

bool FanLightOff() {
    Serial.println("Fan Light turn off ...");
    digitalWrite(relayOne, LOW);   // sets relayOne off
    isRelayOneOn = false;
    return isRelayOneOn;
}

bool FanLowOn() {
    Serial.println("Lan Low turn on ...");
    digitalWrite(relayTwo, LOW);   // sets relayTwo off
    delay(250);
    digitalWrite(relayFour, LOW);   // sets relayFour off
    delay(250);
    digitalWrite(relayThree, HIGH);   // sets relayThree on
    FanHigh->switchStatus = false;
    FanMedium->switchStatus = false;
    //FanLow->switchStatus = true;
    isRelayTwoOn = false;
    isRelayThreeOn = true;
    isRelayFourOn = false;
    return isRelayThreeOn;
}

bool FanLowOff() {
  Serial.println("Fan Low turn off ...");
    digitalWrite(relayThree, LOW);   // sets relayThree off
    digitalWrite(relayTwo, LOW);   // sets relayTwo off
    digitalWrite(relayFour, LOW);   // sets relayFour off
    FanHigh->switchStatus = false;
    FanMedium->switchStatus = false;
    //FanLow->switchStatus = false;
    isRelayTwoOn = false;
    isRelayThreeOn = false;
    isRelayFourOn = false;
    return isRelayThreeOn;
}

//sockets

bool FanMediumOn() {
    Serial.println("Fan Medium turn on ...");
    digitalWrite(relayTwo, LOW);   // sets relayTwo off
    delay(250);
    digitalWrite(relayThree, LOW);   // sets relayThree off
    delay(250);
    digitalWrite(relayFour, HIGH);   // sets relayFour on
    FanHigh->switchStatus = false;
    //FanMedium->switchStatus = false;
    FanLow->switchStatus = false;
    isRelayTwoOn = false;
    isRelayThreeOn = false;
    isRelayFourOn = true;
    return isRelayFourOn;
}

bool FanMediumOff() {
    Serial.println("Fan Medium turn off ...");
    digitalWrite(relayThree, LOW);   // sets relayThree off
    digitalWrite(relayTwo, LOW);   // sets relayTwo off
    digitalWrite(relayFour, LOW);   // sets relayFour off
    FanHigh->switchStatus = false;
    //FanMedium->switchStatus = false;
    FanLow->switchStatus = false;
    isRelayTwoOn = false;
    isRelayThreeOn = false;
    isRelayFourOn = false;
    return isRelayFourOn;
}

bool FanHighOn() {
    Serial.println("Fan High turn on ...");
    digitalWrite(relayFour, LOW);   // sets relayFour off
    delay(250);
    digitalWrite(relayThree, LOW);   // sets relayThree off
    delay(250);
    digitalWrite(relayTwo, HIGH);   // sets relayTwo on
    //FanHigh->switchStatus = false;
    FanMedium->switchStatus = false;
    FanLow->switchStatus = false;
    isRelayTwoOn = true;
    isRelayThreeOn = false;
    isRelayFourOn = false;
    return isRelayTwoOn;
}

bool FanHighOff() {
  Serial.println("Fan High turn off ...");
    digitalWrite(relayThree, LOW);   // sets relayThree off
    digitalWrite(relayTwo, LOW);   // sets relayTwo off
    digitalWrite(relayFour, LOW);   // sets relayFour off
    //FanHigh->switchStatus = false;
    FanMedium->switchStatus = false;
    FanLow->switchStatus = false;
    isRelayTwoOn = false;
    isRelayThreeOn = false;
    isRelayFourOn = false;
    return isRelayTwoOn;
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

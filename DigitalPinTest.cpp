// Complete Instructions: https://RandomNerdTutorials.com/esp8266-nodemcu-digital-inputs-outputs-arduino/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
// set pin numbers
const int buttonPin = D6;     // the number of the pushbutton pin
const int ledPin =  D2;       // the number of the LED pin

// variable for storing the pushbutton status
int buttonState = 0;

void setup() {
  Serial.begin(115200);
  // initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);
  // initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // read the state of the pushbutton value
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH
  if (buttonState == HIGH) {
    // turn LED on
    digitalWrite(ledPin, HIGH);
    Serial.println("Data Received - HIGH");
  } else {
    // turn LED off
    digitalWrite(ledPin, LOW);
    Serial.println("Data Received - LOW");
  }
}
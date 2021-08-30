#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <math.h>
#include <ir_Samsung.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1
#define MQTT_MAX_PACKET_SIZE 512

#include "main.h"


WiFiClient wifiClient;
PubSubClient client(wifiClient);

/*
  Adding AC functionality to current D1 Mini
*/

IRSamsungAc ac(IR_pin);
IRsend irsend(IR_pin);

//Declare prototype functions
void increment();
void decrement();
void update();
void updateServerValue();

void btnSwingInt()
{
  isSwing = !isSwing;
  update();
  updateServerValue();
}

void btnSpeedInt()
{
  fanSpeed = ++fanSpeed % 4;
  update();
  updateServerValue();
}

// function for up-down interrupt
void btnUpInt()
{

  increment();
}

void btnDnInt()
{
  decrement();
}

void changeACmode()
{
  isCool = !isCool;
  update();
  updateServerValue();
}

void update()
{
  static bool isLastOn = false;
  static int lastTemp = 0;
  static bool lastIsSwing = isSwing;
  static int lastFanSpeed = fanSpeed;
  static int lastIsCool = isCool;

  if (setTemp == lastTemp && isSwing == lastIsSwing && lastFanSpeed == fanSpeed && lastIsCool == isCool && isLastOn == isOn)
    return;

  if (isOn)
  {
    ac.on();
    if (isCool)
    {
      Serial.println("Send IR : temp = " + String(setTemp) + " swing = " + String(isSwing) + " Fan Speed : " + String(fanSpeed) + " IsLastOn : " + isLastOn);
      isLastOn = true;
      lastTemp = setTemp;
      lastIsSwing = isSwing;
      lastFanSpeed = fanSpeed;
      lastIsCool = true;

      //Set and send commands
      ac.setMode(kSamsungAcCool);
      switch (fanSpeed)
      {
      case (0):
        ac.setFan(kSamsungAcFanAuto);
        break;
      case (1):
        ac.setFan(kSamsungAcFanLow);
        break;
      case (2):
        ac.setFan(kSamsungAcFanMed);
        break;
      case (3):
        ac.setFan(kSamsungAcFanHigh);
        break;
      }

      ac.setSwing(isSwing);
      ac.setTemp(setTemp);
    }
    else
    {
      ac.setMode(kSamsungAcFan);
      lastIsCool = false;
    }

    ac.send();
  }
  else if (isLastOn)
  {
    Serial.println("Send off");
    isLastOn = false;

    //set and send command
    ac.off();
    irsend.sendRaw(ac_off, 349, 38);      //I need to send raw for off because Library didn't work.
  }
}

void updateServerValue()
{

  /*!!-- Need to redefine MQTT_MAX_PACKET_SIZE 256 --!! */

  String value;
  String message;
  char data[200];

  //Primary
  // message = "{\"name\" : \"" + device_name + "\", \"service_name\" : \"" + service_name + "\", \"characteristic\" : \"CurrentTemperature\", \"value\" : " + String(setTemp) + "}";
  // message.toCharArray(data, (message.length() + 1));
  message = "{\"name\": \"Smart AC\",\"service_name\": \"smart_ac\",\"characteristic\": \"CurrentTemperature\",\"value\":"+String(setTemp)+"}";
  client.publish(mqtt_device_value_to_set_topic, message.c_str());

  message = "{\"name\" : \"" + device_name + "\", \"service_name\" : \"" + service_name + "\", \"characteristic\" : \"Active\", \"value\" : " + String(isOn) + "}";
  message.toCharArray(data, (message.length() + 1));
  client.publish(mqtt_device_value_to_set_topic, data);

  message = "{\"name\" : \"" + device_name + "\", \"service_name\" : \"" + service_name + "\", \"characteristic\" : \"SwingMode\", \"value\" : " + String(isSwing) + "}";
  message.toCharArray(data, (message.length() + 1));
  client.publish(mqtt_device_value_to_set_topic, data);

  message = "{\"name\": \"Smart AC\",\"service_name\": \"smart_ac\",\"characteristic\": \"CoolingThresholdTemperature\",\"value\":" +String(setTemp) + " }";
  client.publish(mqtt_device_value_to_set_topic, message.c_str());

  message = "{\"name\" : \"" + device_name + "\", \"service_name\" : \"" + service_name + "\", \"characteristic\" : \"RotationSpeed\", \"value\" : " + String(fanSpeed) + "}";
  message.toCharArray(data, (message.length() + 1));
  client.publish(mqtt_device_value_to_set_topic, data);

  if (isCool){
     message = "{\"name\": \"Smart AC\",\"service_name\": \"smart_ac\",\"characteristic\": \"CurrentHeaterCoolerState\",\"value\":1}";
     client.publish(mqtt_device_value_to_set_topic, message.c_str());
     message = "{\"name\": \"Smart AC\",\"service_name\": \"smart_ac\",\"characteristic\": \"TargetHeaterCoolerState\",\"value\":2}";
     client.publish(mqtt_device_value_to_set_topic, message.c_str());
  }else{
    message = "{\"name\": \"Smart AC\",\"service_name\": \"smart_ac\",\"characteristic\": \"CurrentHeaterCoolerState\",\"value\":2}";
     client.publish(mqtt_device_value_to_set_topic, message.c_str());
     message = "{\"name\": \"Smart AC\",\"service_name\": \"smart_ac\",\"characteristic\": \"TargetHeaterCoolerState\",\"value\":1}";
     client.publish(mqtt_device_value_to_set_topic, message.c_str());
  }

  //Secondary
  message = "{\"name\" : \"" + device_name_secondary + "\", \"service_name\" : \"" + service_name_secondary + "\", \"characteristic\" : \"On\", \"value\" : " + String(isOn) + "}";
  message.toCharArray(data, (message.length() + 1));
  client.publish(mqtt_device_value_to_set_topic, data);
}

void blink()
{
  //Blink on received MQTT message
  digitalWrite(2, LOW);
  delay(20);
  digitalWrite(2, HIGH);
}

void setup_ota()
{
  // Set OTA Password, and change it in platformio.ini
  ArduinoOTA.setHostname("ESP8266-D1 MINI");
  // ArduinoOTA.setPassword("12345678");
  ArduinoOTA.onStart([]() {
    String type;
    if(ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      //U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Start Updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.printf("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%r",(progress/(total/100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ",error);
    if (error == OTA_AUTH_ERROR)
      ; // Auth failed
    else if (error == OTA_BEGIN_ERROR)
      ; // Begin failed
    else if (error == OTA_CONNECT_ERROR)
      ; // Connect failed
    else if (error == OTA_RECEIVE_ERROR)
      ; // Receive failed
    else if (error == OTA_END_ERROR)
      ; // End failed
  });
  ArduinoOTA.begin();
}

void ACOnOff()
{
  if (!isOn)
  {
    Serial.println("AC On");
    isOn = true;
  }
  else
  {
    Serial.println("AC Off");
    isOn = false;
  }
  update();
}

void increment()
{
  if (!isCool)
    return;
  setTemp++;
  update();
  updateServerValue();
}

void decrement()
{
  if (!isCool)
    return;
  setTemp--;
  update();
  updateServerValue();
}

void btnPowerPressed()
{
  int press_millis = millis();
  while (digitalRead(btnPower) == LOW)
  {
    delay(0);
    if (millis() - press_millis > 1000)   //if long press, change AC Mode
    {
      isCool = !isCool;
      update();
      return;
    }
  };

  ACOnOff();
  updateServerValue();
}


// ******************************AC Function Ends Here *********************************************** //

// function called to publish the state of the light (on/off)
void publishFanState() {
  if (m_fan_state) {
    client.publish(MQTT_FAN_STATE_TOPIC, FAN_ON, true);
  } else {
    client.publish(MQTT_FAN_STATE_TOPIC, FAN_OFF, true);
  }
}

// function called to turn on/off the light
void setFanState() {
  if (m_fan_state) {
    digitalWrite(FAN_PIN, HIGH);
    Serial.println("INFO: Turn fan on...");
  } else {
    digitalWrite(FAN_PIN, LOW);
    Serial.println("INFO: Turn fan off...");
  }
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  Serial.println(p_topic);
  Serial.println(payload);
  // handle message topic
  if (String(MQTT_FAN_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(FAN_ON))) {
      if (m_fan_state != true) {
        m_fan_state = true;
        setFanState();
        publishFanState();
      }
    } else if (payload.equals(String(FAN_OFF))) {
      if (m_fan_state != false) {
        m_fan_state = false;
        setFanState();
        publishFanState();
      }
    }
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
      // Once connected, publish an announcement...
      publishFanState();
      // ... and resubscribe
      client.subscribe(MQTT_FAN_COMMAND_TOPIC);
      
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // init the serial
  Serial.begin(115200);

  // init the led
  pinMode(FAN_PIN, OUTPUT);
  analogWriteRange(1023);
  setFanState();

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  updateServerValue();
  setup_ota();

 //Setup IR Lib
  ac.begin();
  irsend.begin();


  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
}
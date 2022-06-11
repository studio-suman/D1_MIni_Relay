//EJ: 4 Relay Switch over WIFI using MQTT and Home Assistant with OTA Support
//EJ: The only way I know how to contribute to the HASS Community... you guys ROCK!!!!!
//EJ: Erick Joaquin :-) Australia

// Original sketch courtesy of ItKindaWorks
//ItKindaWorks - Creative Commons 2016
//github.com/ItKindaWorks
//
//Requires PubSubClient found here: https://github.com/knolleary/pubsubclient
//
//ESP8266 Simple MQTT switch controller


#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
//#include <IRremoteESP8266.h>
#include <IRrecv.h>



//EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.1.15"  //your MQTT IP Address
#define mqtt_user "mqtt_user" 
#define mqtt_password "password"
#define mqtt_port 1883
const char* ssid = "Suman Saha";
const char* password = "rockylotus428";

//EJ: Data PIN Assignment on WEMOS D1 R2 https://www.wemos.cc/product/d1.html
// if you are using Arduino UNO, you will need to change the "D1 ~ D4" with the corresponding UNO DATA pin number 

const int switchPin1 = D1;
const int switchPin2 = D2;
const int switchPin3 = D3;
const int switchPin4 = D4;

//EJ: These are the MQTT Topic that will be used to manage the state of Relays 1 ~ 4
//EJ: Refer to my YAML component entry
//EJ: feel free to replicate the line if you have more relay switch to control, but dont forget to increment the number suffix so as increase switch logics in loop()

char const* switchTopic1 = "/house2/switch1/";
char const* switchTopic2 = "/house2/switch2/";
char const* switchTopic3 = "/house2/switch3/";
char const* switchTopic4 = "/house2/switch4/";

//Iremote definations for Remote pin


#define code1  16894007 //0x1FED827  // code received from button 2 (STB)
#define code2  16898087 //0x1FEF807  // code received from button 3 (STB)
#define code3  16851167 //0x101C837  // code received from button 4 (STB)
#define code4  33478695 //0x101D827  // code received from button 2 (NEC)
#define code5  33486855 //0x20240BF  // code received from button 3 (NEC)
#define code6  33435855 //0x202C03F  // code received from button 4 (NEC)
#define codeW 0x4053888   // Panasonic button 2
#define codeV 0x4053848   //Panasonic button 3

const int RECV_PIN = D6; // the pin where you connect the output pin of TSOP4838
int itsONled[] = {0,0,0,0,0};

IRrecv irrecv(RECV_PIN); // create a new instance of the IR receiver

//IrReceiver;
 
decode_results results;
//IrReceiver.decodedIRData

void callback(char* topic, byte* payload, unsigned int length);

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, mqtt_port, callback, wifiClient);

//generate unique name from MAC addr
String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}


void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED){
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }



  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName = "esp8266-House1";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      //EJ: Delete "mqtt_username", and "mqtt_password" here if you are not using any 
      if (client.connect((char*) clientName.c_str(),mqtt_user, mqtt_password)) {  //EJ: Update accordingly with your MQTT account 
      //if (client.connect(mqtt_user, mqtt_password)) {
        Serial.print("\tMQTT Connected");
        client.subscribe(switchTopic1);
        client.subscribe(switchTopic2);
        client.subscribe(switchTopic3);
        client.subscribe(switchTopic4);
        //EJ: Do not forget to replicate the above line if you will have more than the above number of relay switches
      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
    }
  }
}



void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 
  //EJ: Note:  the "topic" value gets overwritten everytime it receives confirmation (callback) message from MQTT

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

   if (topicStr == "/house/switch1/") 
    {

     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == '1'){
       digitalWrite(switchPin1, HIGH);
       client.publish("/house/switchConfirm1/", "1");
       }

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == '0'){
       digitalWrite(switchPin1, LOW);
       client.publish("/house/switchConfirm1/", "0");
       }
     }

     // EJ: copy and paste this whole else-if block, should you need to control more switches
     else if (topicStr == "/house/switch2/") 
     {
     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == '1'){
       digitalWrite(switchPin2, HIGH);
       client.publish("/house/switchConfirm2/", "1");
       }

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == '0'){
       digitalWrite(switchPin2, LOW);
       client.publish("/house/switchConfirm2/", "0");
       }
     }
     else if (topicStr == "/house/switch3/") 
     {
     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == '1'){
       digitalWrite(switchPin3, HIGH);
       client.publish("/house/switchConfirm3/", "1");
       }

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == '0'){
       digitalWrite(switchPin3, LOW);
       client.publish("/house/switchConfirm3/", "0");
       }
     }
     else if (topicStr == "/house/switch4/") 
     {
     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == '1'){
       digitalWrite(switchPin4, HIGH);
       client.publish("/house/switchConfirm4/", "1");
       }

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == '0'){
       digitalWrite(switchPin4, LOW);
       client.publish("/house/switchConfirm4/", "0");
       }
     }
     else if (topicStr == "/house2/switch1/") 
     {
     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == '1'){
       digitalWrite(switchPin1, HIGH);
       client.publish("/house2/switchConfirm1/", "1");
       }

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == '0'){
       digitalWrite(switchPin1, LOW);
       client.publish("/house2/switchConfirm1/", "0");
       }
     }
     else if (topicStr == "/house2/switch2/") 
     {
     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == '1'){
       digitalWrite(switchPin2, HIGH);
       client.publish("/house2/switchConfirm2/", "1");
       }

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == '0'){
       digitalWrite(switchPin2, LOW);
       client.publish("/house2/switchConfirm2/", "0");
       }
     }
     else if (topicStr == "/house2/switch3/") 
     {
     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == '1'){
       digitalWrite(switchPin3, HIGH);
       client.publish("/house2/switchConfirm3/", "1");
       }

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == '0'){
       digitalWrite(switchPin3, LOW);
       client.publish("/house2/switchConfirm3/", "0");
       }
     }
     else if (topicStr == "/house2/switch4/") 
     {
     //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
     if(payload[0] == '1'){
       digitalWrite(switchPin4, HIGH);
       client.publish("/house2/switchConfirm4/", "1");
       }

      //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
     else if (payload[0] == '0'){
       digitalWrite(switchPin4, LOW);
       client.publish("/house2/switchConfirm4/", "0");
       }
     }
}

void ir_action() {
if (irrecv.decode(&results)) {
  //Serial.println(results.decode_type);
      switch (results.decode_type) {
          case NEC:
            {
    //unsigned int value = results.value;
              switch(results.value) {
                 case code1:
                   if(itsONled[1] == 1) {        // if first led is on then
                      Serial.println("Turning on/off Relay 1: Value Received : ");
                      Serial.println(results.value,HEX);
                      digitalWrite(switchPin1, LOW);   // turn it off when button is pressed
                      itsONled[1] = 0;           // and set its state as off
                   } else {                      // else if first led is off
                       digitalWrite(switchPin1, HIGH); // turn it on when the button is pressed
                       itsONled[1] = 1;          // and set its state as on
                   }
                    break; 
                 case code2:
                   if(itsONled[2] == 1) {
                      Serial.println("Turning on/off Relay 2: Value Received : ");
                      Serial.println(results.value,HEX);
                      digitalWrite(switchPin2, LOW);
                      itsONled[2] = 0;
                   } else {
                       digitalWrite(switchPin2, HIGH);
                       itsONled[2] = 1;
                   }
                    break;       
                case code3:
                   if(itsONled[3] == 1) {
                    Serial.println("Turning on/off Relay 3: Value Received : ");
                      digitalWrite(switchPin3, LOW);
                      itsONled[3] = 0;
                   } else {
                       digitalWrite(switchPin3, HIGH);
                       itsONled[3] = 1;
                   }
                    break; 
                case code4:
                   if(itsONled[1] == 1) {
                      Serial.println("Turning on/off Relay 1(NEC2): Value Received : ");
                      Serial.println(results.value,HEX);
                      digitalWrite(switchPin1, LOW);
                      itsONled[1] = 0;
                   } else {
                       digitalWrite(switchPin1, HIGH);
                       itsONled[1] = 1;
                   }
                    break;  
                case code5:
                   if(itsONled[2] == 1) {
                      Serial.println("Turning on/off Relay 2(NEC2): Value Received : ");
                      Serial.println(results.value,HEX);
                      digitalWrite(switchPin2, LOW);
                      itsONled[2] = 0;
                   } else {
                       digitalWrite(switchPin2, HIGH);
                       itsONled[2] = 1;
                   }
                    break;  
               case code6:
                   if(itsONled[3] == 1) {
                      Serial.println("Turning on/off Relay 3(NEC2): Value Received : ");
                      Serial.println(results.value,HEX);
                      digitalWrite(switchPin3, LOW);
                      itsONled[3] = 0;
                   } else {
                       digitalWrite(switchPin3, HIGH);
                       itsONled[3] = 1;
                   }
                    break;      
                }
        break;
        }
      case PANASONIC:
        {
          switch(results.value) {
                 case codeV:
                   if(itsONled[1] == 1) {        // if first led is on then
                      Serial.println("Turning on/off Relay 1: Value Received : ");
                      Serial.println(results.value,HEX);
                      digitalWrite(switchPin1, LOW);   // turn it off when button is pressed
                      itsONled[1] = 0;           // and set its state as off
                   } else {                      // else if first led is off
                       digitalWrite(switchPin1, HIGH); // turn it on when the button is pressed
                       itsONled[1] = 1;          // and set its state as on
                   }
                    break; 
                 case codeW:
                   if(itsONled[2] == 1) {
                      Serial.println("Turning on/off Relay 2: Value Received : ");
                      Serial.println(results.value,HEX);
                      digitalWrite(switchPin2, LOW);
                      itsONled[2] = 0;
                   } else {
                       digitalWrite(switchPin2, HIGH);
                       itsONled[2] = 1;
                   }
        }
    break;
      } 
    }
  irrecv.resume();  
  delay(100);
  }
}  

void setup() {
  //initialize the switch as an output and set to LOW (off)
  pinMode(switchPin1, OUTPUT); // Relay Switch 1
  digitalWrite(switchPin1, LOW);

  pinMode(switchPin2, OUTPUT); // Relay Switch 2
  digitalWrite(switchPin2, LOW);

  pinMode(switchPin3, OUTPUT); // Relay Switch 3
  digitalWrite(switchPin3, LOW);

  pinMode(switchPin4, OUTPUT); // Relay Switch 4
  digitalWrite(switchPin4, LOW);

  //ArduinoOTA.setHostname("My Arduino WEMO"); // A name given to your ESP8266 module when discovering it as a port in ARDUINO IDE
  ArduinoOTA.begin(); // OTA initialization

  //start the serial line for debugging
  Serial.begin(115200);
  delay(100);
  irrecv.enableIRIn();
  //start wifi subsystem
  WiFi.begin(ssid, password);
  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  //wait a bit before starting the main loop
      delay(2000);
    Serial.println(F("START " __FILE__ " from " __DATE__));

    // In case the interrupt driver crashes on setup, give a clue
    // to the user what's going on.
    Serial.println("Enabling IRin");
    irrecv.enableIRIn(); // Start the receiver

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(RECV_PIN);    Serial.println(F("START " __FILE__ " from " __DATE__));

    // In case the interrupt driver crashes on setup, give a clue
    // to the user what's going on.
    Serial.println("Enabling IRin");
    irrecv.enableIRIn(); // Start the receiver

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(RECV_PIN);  


}


void loop(){

  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

  //maintain MQTT connection
  client.loop();

/*    if (irrecv.decode(&results)) {
    unsigned int ircode = results.value;
    Serial.println(ircode);
    irrecv.resume();  // Receive the next value
  }
  delay(100);
 */
  ir_action();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 
  ArduinoOTA.handle();


}






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
//98:F4:AB:DA:A1:52 MAC fr ES8266
//e8:d8:7e:f7:c5:dd MAC fr Suman Home

//#include <TelnetStream.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h> 
//#include <IRremoteESP8266.h>
//#include <IRrecv.h>
//#include <RemoteDebug.h>  //https://github.com/JoaoLopesF/RemoteDebug

//RemoteDebug Debug;

//EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.1.15"  //your MQTT IP Address
#define mqtt_user "mqtt_user" 
#define mqtt_password "password"
#define mqtt_port 1883
const char* ssid = "Suman Saha";
const char* password = "rockylotus428";
#define HOST_NAME "ESP8266-Suman"

//EJ: Data PIN Assignment on WEMOS D1 R2 https://www.wemos.cc/product/d1.html
// if you are using Arduino UNO, you will need to change the "D1 ~ D4" with the corresponding UNO DATA pin number 

const int switchPin1 = D1;
const int switchPin2 = D2;
const int switchPin3 = D3;
const int switchPin4 = D4;

//EJ: These are the MQTT Topic that will be used to manage the state of Relays 1 ~ 4
//EJ: Refer to my YAML component entry
//EJ: feel free to replicate the line if you have more relay switch to control, but dont forget to increment the number suffix so as increase switch logics in loop()

char const* switchTopic1 = "/house/switch1/"; //Tubelight - suman
char const* switchTopic2 = "/house/switch2/"; //Fan - suman
char const* switchTopic3 = "/house/switch3/"; //NightLamp - suman
char const* switchTopic4 = "/house/switch4/"; //Not Used


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
        Serial.print("\tMQTT Connected\n");
        client.subscribe(switchTopic1);
        client.subscribe(switchTopic2);
        client.subscribe(switchTopic3);
        client.subscribe(switchTopic4);
        //EJ: Do not forget to replicate the above line if you will have more than the above number of relay switches
      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
      //else{Serial.println("\tFailed.");} // Updated Code to abort() instead of looping
    }
  }
}


//Handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 
  //EJ: Note:  the "topic" value gets overwritten everytime it receives confirmation (callback) message from MQTT

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

 // EJ: copy and paste this whole else-if block, should you need to control more switches  
   if (topicStr == "/house/switch1/") //Tubelight - suman
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

  /*
  else if (topicStr == "/house/switch2/") //Fan - suman
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
*/
  else if (topicStr == "/house/switch3/") //NightLamp - suman
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

  else if (topicStr == "/house/switch4/") //Not Used
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

 

  ArduinoOTA.setHostname(HOST_NAME); // A name given to your ESP8266 module when discovering it as a port in ARDUINO IDE
  ArduinoOTA.begin(); // OTA initialization

  //Debug.begin(HOST_NAME); // Debugging initialization
  //Debug.setResetCmdEnabled(true);

  //start the serial line for debugging

  //TelnetStream.begin();
  
  Serial.begin(115200);
  delay(100);
  //irrecv.enableIRIn();
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
    //irrecv.enableIRIn(); // Start the receiver

    Serial.println(F("START " __FILE__ " from " __DATE__));
    Serial.print(F("Ready to receive IR signals at pin "));
    //Serial.println(RECV_PIN);    
    
    // In case the interrupt driver crashes on setup, give a clue
    // to the user what's going on.
    Serial.println("Enabling IRin");
    //irrecv.enableIRIn(); // Start the receiver

    Serial.print(F("Ready to receive IR signals at pin "));
    //Serial.println(RECV_PIN);


}


void loop() {

  //ir_action(); //calling Ir Action

  //Debug.handle(); // Debugging loop  
  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {
    reconnect();
    }

  //maintain MQTT connection
  client.loop();

 /*  switch (TelnetStream.read())
  {
  case 'R':
    TelnetStream.stop();
    delay(100);
    ESP.reset();
    break;
  case 'C':
    TelnetStream.println("bye bye");
    TelnetStream.flush();
    TelnetStream.stop();
    break;
  } */

  //ir_action(); //Prev Position calling Ir Action

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 
  ArduinoOTA.handle();
}
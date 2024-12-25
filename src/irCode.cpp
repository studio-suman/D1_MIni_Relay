/*
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
                      Serial.print(results.value,HEX);
                      digitalWrite(switchPin1, LOW);   // turn it off when button is pressed
                      itsONled[1] = 0;           // and set its state as off
                      client.publish("/house2/switchConfirm1/", "0");
                   } else {                      // else if first led is off
                       digitalWrite(switchPin1, HIGH); // turn it on when the button is pressed
                       itsONled[1] = 1;          // and set its state as on
                       client.publish("/house2/switchConfirm1/", "1");
                   }
                    break; 
                 case code2:
                   if(itsONled[2] == 1) {
                      Serial.println("Turning on/off Relay 2: Value Received : ");
                      Serial.print(results.value,HEX);
                      digitalWrite(switchPin2, LOW);
                      itsONled[2] = 0;
                      client.publish("/house2/switchConfirm2/", "0");
                   } else {
                       digitalWrite(switchPin2, HIGH);
                       itsONled[2] = 1;
                       client.publish("/house2/switchConfirm2/", "1");
                   }
                    break;       
                case code3:
                   if(itsONled[3] == 1) {
                    Serial.println("Turning on/off Relay 3: Value Received : ");
                    Serial.print(results.value,HEX);
                      digitalWrite(switchPin3, LOW);
                      itsONled[3] = 0;
                      client.publish("/house2/switchConfirm1/", "0");
                   } else {
                       digitalWrite(switchPin3, HIGH);
                       itsONled[3] = 1;
                       client.publish("/house2/switchConfirm3/", "1");
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


*/
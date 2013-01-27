#include <SoftwareSerial.h>
#include "Wire.h"

#define I2C_ADDR  0x20  // 0x20 is the address with all jumpers removed

//uint8_t pinRx = 2, pinTx =3;
SoftwareSerial mySerial(2, 3);
SoftwareSerial impSerial(8, 9);

long BaudRate = 4800;
unsigned long start, finished, elapsed; //time variables
String sensorstring = "";
boolean sensor_stringcomplete = false;
int ledON = 4;
int ledOFF = 5;


void setup()
{
  
  Serial.begin(BaudRate);
  //Serial.println("RELAY8 demonstration starting up");

  Wire.begin(); // Wake up I2C bus

  // Set addressing style
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x12);
  Wire.write(0x20); // use table 1.4 addressing
  Wire.endTransmission();

  // Set I/O bank A to outputs
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // Set all of bank A to outputs
  Wire.endTransmission();
  
  //Serial.println("Ready. Type 0 to turn off relays, 1 - 8 to activate a relay.");
  
  impSerial.begin(19200); //electric imp serial comms
  mySerial.begin(BaudRate);  //xbee comms
  mySerial.listen();  //begin listening for comms
  pinMode(ledON, OUTPUT); //display that the water system is on
  pinMode(ledOFF, OUTPUT); //display that the system is on and the water is off
  digitalWrite(ledON, HIGH);
}


void loop()
{

      // receive
      while (mySerial.available()) {  
        
             char inchar = (char)mySerial.read();   
             sensorstring += inchar; 

             if (inchar == '\r') {sensor_stringcomplete = true;}
     }



    if (sensor_stringcomplete){ 
            blinkLED(ledON);   
    
            sensor_stringcomplete = false;  //reset  
         
            if (sensorstring[1] == 't') {
              Serial.print("Temperature of the sensor unit: ");
              Serial.println(sensorstring.substring(3));
    
            }
            else if (sensorstring[1] == 'c') {
              Serial.print("Charing Status: ");
              Serial.println(sensorstring.substring(3));
    
            }
            else if (sensorstring[1] == 'w') {
                if (sensorstring[3] == '1') {
                   Serial.print("Water sensor reading: ");
                   Serial.println(sensorstring.substring(5)); //get water moisture reading
                   Serial.println("Water is on"); //valve 
                   sendValueToLatch(1); //trigger relay to turn on solenoid
                   //impSerial.write(sensorstring); //post to electric imp and cosm from there
                   digitalWrite(ledOFF, HIGH);                   
                }
                else if (sensorstring[3] == '0') {
                   Serial.print("Water sensor reading: ");
                   Serial.println(sensorstring.substring(5)); //get water moisture reading
                   Serial.println("Water is off");
                   sendValueToLatch(0); // turn off water valve
                   //impSerial.write("0");
                   digitalWrite(ledOFF, LOW);
               }
          }
          else { //backup
               Serial.print("Value: ");
                Serial.println(sensorstring[1]);
            }
            Serial.println(sensorstring);
            impSerial.print(sensorstring); //post to electric imp and cosm from there
            
            sensorstring = ""; //reset string value
    
            
    }
       
  
  //relay control code
  
  int command = 0;
   if (Serial.available()) {
    command = Serial.read();
    if( command == '0' )
    {
      sendValueToLatch(0);
//Serial.println("Resetting all relays");
    }
    if( command == '1' )
    {
      sendValueToLatch(1);
      //Serial.println("Activating relay 1");
    }
    if( command == '2' )
    {
      sendValueToLatch(2);
//Serial.println("Activating relay 2");
    }
    if( command == '3' )
    {
      sendValueToLatch(4);
//Serial.println("Activating relay 3");
    }
    if( command == '4' )
    {
      sendValueToLatch(8);
//Serial.println("Activating relay 4");
    }
    if( command == '5' )
    {
      sendValueToLatch(16);
      //Serial.println("Activating relay 5");
    }
    if( command == '6' )
    {
      sendValueToLatch(32);
      Serial.println("Activating relay 6");
    }
    if( command == '7' )
    {
      sendValueToLatch(64);
      Serial.println("Activating relay 7");
    }
    if( command == '8' )
    {
      sendValueToLatch(128);
     // Serial.println("Activating relay 8");
    }
  }

}

void sendValueToLatch(int latchValue)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x12);        // Select GPIOA
  Wire.write(latchValue);  // Send value to bank A
  Wire.endTransmission();
}

void blinkLED(int ledValue) {
     digitalWrite(ledValue, LOW); //signal that we are receiving something
     delay(100);
      digitalWrite(ledValue, HIGH); //signal that we are receiving something
     delay(100);
      digitalWrite(ledValue, LOW); //signal that we are receiving something
     delay(100);
      digitalWrite(ledValue, HIGH); //signal that we are receiving something
     delay(100);
      digitalWrite(ledValue, LOW); //signal that we are receiving something
       delay(100);
      digitalWrite(ledValue, HIGH); //signal that we are receiving something
}




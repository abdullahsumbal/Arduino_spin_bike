#include "Wire.h"
#include <TimeLib.h>
#include <Time.h>

#define EEPROM_ADDRESS 0x50
#define EEPROM_START_ADDRESS 0
#define DATA_SIZE 6

void setup(){
  Wire.begin();
  Serial.begin(9600);
  pinMode(A3, OUTPUT);
  digitalWrite(A3, HIGH);
  delay(1000);
   Serial.println("active");
displayData();

}


void displayData(){
  int address = 0;
   while(!(readEEPROM(EEPROM_ADDRESS, address) == 255 &&
  
        readEEPROM(EEPROM_ADDRESS, address+1) == 255 &&      
        readEEPROM(EEPROM_ADDRESS, address+2) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+3) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+4) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+5) == 255)){
           Serial.print(address/6+1);
           Serial.print(",");
          for (int i =0; i<5;i++){
            byte data = readEEPROM(EEPROM_ADDRESS, address+i);
            if(data < 10) Serial.print(0);
            Serial.print(data);
          }
          Serial.print(",");
          Serial.print(readEEPROM(EEPROM_ADDRESS, address+5));
          Serial.println();
         address += DATA_SIZE;
        }
}


void loop(){
  
}

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) 
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
 
  delay(5);
}
 
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) 
{
  byte rdata = 0xFF;
 
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(deviceaddress,1);
 
  if (Wire.available()) rdata = Wire.read();
 
  return rdata;
}

int startAddress(){
  int address = EEPROM_START_ADDRESS;
  while(!(readEEPROM(EEPROM_ADDRESS, address) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+1) == 255 &&      
        readEEPROM(EEPROM_ADDRESS, address+2) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+3) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+4) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+5) == 255)){
         address += DATA_SIZE;
        }
        
   return address;
}

void clearMemory(){
    for (int address = 0; address < 8*1024; address++ ){
      if (readEEPROM(EEPROM_ADDRESS, address) != 255){
        Serial.print("clearing memory");
        Serial.println(address);
        writeEEPROM(EEPROM_ADDRESS, address,255 );
      }
  }
}

void digitalClockDisplay(time_t time) {
 // digital clock display of the time
 Serial.print(hour(time));
 Serial.print(":");
 Serial.print(minute(time));
 Serial.print(":");
 Serial.print(second(time));
 Serial.print(" ");
 Serial.print(month(time));
 Serial.print(" ");
 Serial.print(year(time));
}



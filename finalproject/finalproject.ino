#include "Wire.h"
#include <TimeLib.h>
#include <Time.h>

#define DS3231_I2C_ADDRESS 0x68
#define EEPROM_ADDRESS 0x50
#define WAIT_UNTIL_NEXT_USER 5
#define EEPROM_START_ADDRESS 0
#define DATA_SIZE 6
#define MINIMUM_USE_TIME 5
#define WRITE_DELAY 1000

volatile byte someoneOnBike = 0;
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  //setDS3231time(55,59,10,1,28,5,17);
  
  //Save Power by writing all Digital IO LOW - note that pins just need to be tied one way or another, do not damage devices!
  for (int i = 0; i < 20; i++) {
    if(i != 2)//just because the button is hooked up to digital pin 2
    pinMode(i, OUTPUT);
  }
  pinMode(A3, OUTPUT);
  digitalWrite(A3, HIGH);
  
  // clear up memory. may want to comment this, if need to keep old data
  //clearMemory();
  
  // display data already in the memory
  displayData();
  Serial.println("active");
  
  // set up interrupt to detect pedale movement.
  attachInterrupt(0, digitalInterrupt, CHANGE); //interrupt for waking up


}

void loop()
{
  //displayTime(); // display the real-time clock data on the Serial Monitor,
  if (someoneOnBike) recordTime();
  //Serial.println("going to sleep");
  delay(5000); // every second
  
  //Serial.print("going to sleep");
  //Disable ADC - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
  ADCSRA &= ~(1 << 7);
  
  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep
  
    //BOD DISABLE - this must be called right before the __asm__ sleep instruction
  MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
  __asm__  __volatile__("sleep");//in line assembler to go to sleep
  
}

void digitalInterrupt(){
  //needed for the digital input interrupt
  someoneOnBike = 1;
}

void recordTime(){
  syncTime();
  time_t timeStart = now();
  
  // Read time until user is padeling the bike.
  byte pedaling =1;
  while(pedaling){
    /// uncomment to debug
     //Serial.println("Someone starting biking");
    pedaling = 0;
    byte pedaleNotAtBottom = 0;
    byte pedaleAtBottom = 0;
  
    // time loop of 3 seconds
    time_t currentTime = now();
    while(currentTime + WAIT_UNTIL_NEXT_USER > now() ){
      
      // High on interrupt pin == someone pedaling
      int x = digitalRead(2);
      if ( x == 1){
        //Serial.println("Someone is using the bike");
        pedaling = 1;
        pedaleAtBottom = 1;
      }
      // make sure padel does not remian at the bottom
      else{
          pedaleNotAtBottom = 1;
      }
    }
    
    // stop recording time if pedale state is not changing
    if (!(pedaleNotAtBottom && pedaleAtBottom)){
       pedaling = 0;
    }
  }
  syncTime();
  time_t timeEnd = now(); 
  if(timeEnd - timeStart > MINIMUM_USE_TIME){
    writeTime(timeStart, timeEnd);
  }
}

void writeTime(time_t timeStart, time_t timeEnd){

  byte unixTime [5];
  time_t copyStartTime = timeStart;
   for (int i = 0; i < 5; i++){
     unixTime[4-i] = copyStartTime % 100;
     copyStartTime = copyStartTime / 100;
   }
   
   int diff_int = timeEnd - timeStart;
   byte diff_byte;
   if (diff_int/60 < 255){
     diff_byte = diff_int/60; // dont forget to divide it by 60
   }
   else{
     diff_byte = 255;
   }
   // uncomment fo rdebuging
   //Serial.print("time used seconds ");
   //Serial.println(diff_byte);
   //Serial.print("start time ");
   //Serial.println(timeStart);
   //Serial.print("end time ");
   //Serial.println(timeEnd);
  someoneOnBike =0;
  
  // writing to eeprom
   int  address = startAddress();
   for(int i = 0; i < 5; i++){
    writeEEPROM(EEPROM_ADDRESS, address, unixTime[i]);
    address+= 1;
    delay(WRITE_DELAY);
   }
   writeEEPROM(EEPROM_ADDRESS, address, diff_byte);
       
   // write to console
   Serial.print(timeStart);Serial.print(",");Serial.println(diff_byte);
}

void displayData(){
  int address = 0;
   while(!(readEEPROM(EEPROM_ADDRESS, address) == 255 &&
  
        readEEPROM(EEPROM_ADDRESS, address+1) == 255 &&      
        readEEPROM(EEPROM_ADDRESS, address+2) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+3) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+4) == 255 &&
        readEEPROM(EEPROM_ADDRESS, address+5) == 255)){
          for (int i =0; i<5;i++){
            byte data = readEEPROM(EEPROM_ADDRESS, address+i);
            if(data < 10) Serial.print(0);
            Serial.print(data);
          }
          Serial.print(",");
          Serial.println(readEEPROM(EEPROM_ADDRESS, address+5));
         address += DATA_SIZE;
        }
}











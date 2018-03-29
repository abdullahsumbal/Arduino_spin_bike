///* */
//#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
// 
//const char* ssid = "Sumbal";
//const char* password = "12345678";
// 
//void setup () {
// 
//  Serial.begin(115200);
//  WiFi.begin(ssid, password);
// 
//  while (WiFi.status() != WL_CONNECTED) {
// 
//    delay(1000);
//    Serial.print("Connecting..");
// 
//  }
// 
//}
// 
//void loop() {
// 
//  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
// 
//    HTTPClient http;  //Declare an object of class HTTPClient
// 
//    http.begin("http://spin-bike-api.herokuapp.com/hardware?sb_id=2&duration=54");  //Specify request destination
//    int httpCode = http.GET();                                                                  //Send the request
//      Serial.println(httpCode);
//    if (httpCode > 0) { //Check the returning code
// 
//      String payload = http.getString();   //Get the request response payload
//      Serial.println(payload);                     //Print the response payload
// 
//    }
// 
//    http.end();   //Close connection
// 
//  }
// 
//  delay(30000);    //Send a request every 30 seconds
// 
//}

#include <Time.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define WAIT_UNTIL_NEXT_USER 5
#define MINIMUM_USE_TIME 5
#define READ_DIGITAL_PIN 0

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
const char* ssid = "Sumbal";
const char* password = "12345678";

void setup() {

  // Conenct to netwrok
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print("Connecting..");
 
  }
}

// the loop function runs over and over again forever
void loop() {

  // Initial Digital Read 
  int initRead = digitalRead(READ_DIGITAL_PIN);
  Serial.println(initRead);
  sendData(134);
  test();
  delay(100);


}

void test(){
  // Inner forever loop
    int initRead = digitalRead(READ_DIGITAL_PIN);
    Serial.println(initRead);
    delay(100);

  while(1){
    Serial.println(3);
    delay(100);
    // Read the current digital value
    int curRead = digitalRead(READ_DIGITAL_PIN);

    // initially no one is peddaling
    int pedaling = 0;

    // Check if the input changes
    if (curRead != initRead){
        pedaling = 1;
    }
    // Record start time 
    time_t timeStart = now();

    // pedaling loop
  while(pedaling){
    /// uncomment to debug
    Serial.println(4);
    delay(100);
    pedaling = 0;
    byte pedaleNotAtBottom = 0;
    byte pedaleAtBottom = 0;
  
    // time loop of 5 seconds
    time_t currentTime = now();
    while(currentTime + WAIT_UNTIL_NEXT_USER > now() ){
      delay(100);
      
      // High on interrupt pin == someone pedaling
      int x = digitalRead(READ_DIGITAL_PIN);
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
  time_t timeEnd = now(); 
  if(timeEnd - timeStart > MINIMUM_USE_TIME){
    sendData(timeEnd - timeStart);
  }
  }
}

void sendData(int sec)
{
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
 
    http.begin("http://spin-bike-api.herokuapp.com/hardware?sb_id=2&duration=" + String(sec));  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
      Serial.println(httpCode);
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
 
    }
 
    http.end();   //Close connection
 
  }
  delay(5000);
}



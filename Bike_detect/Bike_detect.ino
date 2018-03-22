#include <Time.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>           // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define WAIT_UNTIL_NEXT_USER 5
#define MINIMUM_USE_TIME 5
#define READ_DIGITAL_PIN 0

IPAddress server_addr(35,199,35,135);  // IP of the MySQL *server* here
char user[] = "root";              // MySQL user login username
char password[] = "need_pants";        // MySQL user login password

// Sample query
char INSERT_SQL[] = "INSERT INTO sbg_db.location (l_id,room_num, address) VALUES (22,'2nd','Respath')";

// WiFi card example
char ssid[] = "Sumbal";         // your SSID
char pass[] = "12345678";     // your SSID Password

WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor cursor(&conn);

void setup() {
    Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  pinMode(READ_DIGITAL_PIN, INPUT);
}

// the loop function runs over and over again forever
void loop() {

  // Initial Digital Read 
  int initRead = digitalRead(READ_DIGITAL_PIN);
  Serial.println(initRead);
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
  if (conn.connected())
  {
    Serial.println("executing query");
    char INSERT_SQL[]  = "Insert into sbg_db.bike_usage values (2, '2018-10-14 13:25:24', 91);";
    boolean result = cursor.execute(INSERT_SQL);
    Serial.println(6);
  }
  delay(5000);
}


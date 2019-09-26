/**
   Read data from DHT22 using arduino
   Parse data to json
   Then transfer to nodemcu
   Nodemcu get data from Arduino then upload to thingspeak server
*/

#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

#define LED D0

// Network parameters
const char* ssid     = "A Di Da Phat";
const char* password = "nammoadidaphat";

// ThingSpeak information
char thingSpeakAddress[] = "api.thingspeak.com";
unsigned long channelID = 866214;
char* readAPIKey = "5CWD3TBZG3A5UNPG";
char* writeAPIKey = "6QFHSB8F3K2AEMXL";
const unsigned long postingInterval = 120L * 1000L;

unsigned long lastConnectionTime = 0;
long lastUpdateTime = 0;
WiFiClient client;
unsigned int dataFieldThree = 3;  

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  //  Serial.begin(115200);
  Serial.println("Start");
  pinMode(LED, OUTPUT);
  connectWiFi();

}

void loop() {

  int light = readTSData( channelID, dataFieldThree );
  Serial.print("Read from server: Light=");
  Serial.print(light);
  turnLight(light);
  delay(5000);
}

void turnLight(int light) {
  if (light == 0) {
    digitalWrite(LED, LOW);
  }
  else {
    digitalWrite(LED, HIGH);
  }
  delay(1000);
}

int connectWiFi() {

  while (WiFi.status() != WL_CONNECTED) {

    WiFi.begin( ssid, password );
    delay(2500);
    Serial.println("Connecting to WiFi");
  }

  Serial.println( "Connected" );
  ThingSpeak.begin( client );
}
float readTSData( long TSChannel, unsigned int TSField ) {

  float data =  ThingSpeak.readFloatField( TSChannel, TSField, readAPIKey );
  Serial.println( " Data read from ThingSpeak: " + String( data, 9 ) );
  return data;

}

// Use this function if you want to write a single field.
int writeTSData( long TSChannel, unsigned int TSField, float data ) {
  int  writeSuccess = ThingSpeak.writeField( TSChannel, TSField, data, writeAPIKey ); // Write the data to the channel
  if ( writeSuccess ) {

    Serial.println( String(data) + " written to Thingspeak." );
  }

  return writeSuccess;
}

// Use this function if you want to write multiple fields simultaneously.
int write2TSData( long TSChannel, unsigned int TSField1, int field1Data, unsigned int TSField2, int field2Data) {

  ThingSpeak.setField( TSField1, field1Data );
  ThingSpeak.setField( TSField2, field2Data );
  int writeSuccess = ThingSpeak.writeFields( TSChannel, writeAPIKey );
  if ( writeSuccess ) {
    Serial.println("Written to Thingspeak." );
  }
  return writeSuccess;
}

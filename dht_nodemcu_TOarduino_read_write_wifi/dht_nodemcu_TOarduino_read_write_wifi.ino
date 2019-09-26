/**
 * Read data from DHT22 using arduino
   Parse data to json
   Then transfer to nodemcu
   Nodemcu get data from Arduino then upload to thingspeak server 
 */
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

SoftwareSerial s(D6, D5);
// Network parameters
const char* ssid     = "A Di Da Phat";
const char* password = "nammoadidaphat";

// ThingSpeak information
char thingSpeakAddress[] = "api.thingspeak.com";
unsigned long channelID = 866214;
char* readAPIKey = "5CWD3TBZG3A5UNPG";
char* writeAPIKey = "6QFHSB8F3K2AEMXL";
const unsigned long postingInterval = 120L * 1000L;
unsigned int dataFieldOne = 1;                        // Field to write temperature data
unsigned int dataFieldTwo = 2;                       // Field to write humidity data
unsigned int dataFieldThree = 3;  

unsigned long lastConnectionTime = 0;
long lastUpdateTime = 0;
WiFiClient client;

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  //  Serial.begin(115200);
  Serial.println("Start");
  s.begin(9600);
  while (!Serial) continue;

  connectWiFi();

  // Read the constants at startup.
  int temperature = readTSData( channelID, dataFieldOne );
  int hum = readTSData( channelID, dataFieldTwo );
  Serial.println("Read data from server");
  Serial.println("Temperature=" + temperature);
  Serial.println("Humidity=" + hum);
}

void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid())
    return;

  Serial.println("JSON received from Arduino and parsed");
  root.prettyPrintTo(Serial);
  Serial.print("Temperature ");
  int temp = root["temperature"];
  Serial.print(temp);
  Serial.print("   Humidity ");
  int  hum = root["humidity"];
  Serial.print(hum);
  Serial.println("");
  Serial.println("---------------------xxxxx--------------------");

  // Update only if the posting time is exceeded
  if (millis() - lastUpdateTime >=  postingInterval) {
    lastUpdateTime = millis();
    write2TSData( channelID , dataFieldOne , temp , dataFieldTwo , hum );      // Write the temperature in F, C, and time since starting.
  }

//  int light = readTSData( channelID, dataFieldThree );
//  Serial.print("Read from server: Light=");
//  Serial.print(light);
//  sendLight(light);
}

void sendLight(int light){
   // send data to arduino
  StaticJsonBuffer<1000> jsonBufferLed;
  JsonObject& rootLed = jsonBufferLed.createObject();
  rootLed["light"] = light;
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

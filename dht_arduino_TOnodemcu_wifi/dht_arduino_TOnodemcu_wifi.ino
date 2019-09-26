/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
   ----------------
   Read data from DHT22 using arduino
   Parse data to json
   Then transfer to nodemcu
   Nodemcu get data from Arduino then upload to thingspeak server
*/

//Libraries
#include <DHT.h>;
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial s(5, 6);

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
int led = 13;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  s.begin(9600);
  pinMode(led, OUTPUT);
}

void loop()
{
  delay(2000);
 
  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temperature: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  // send data to nodemcu
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = temp;
  root["humidity"] = hum;
  if (s.available() > 0)
  {
    root.printTo(s);
  }

   // read light send from server
//  int light = readLight();
//  turnLight(light);
  
  delay(10000);
}

void turnLight(int light) {
  if (light == 0) {
    Serial.println("Turn light off");
    digitalWrite(led, LOW);
  }
  else {
     Serial.println("Turn light on");
    digitalWrite(led, HIGH);
  }
}
int readLight() {
  StaticJsonBuffer<1000> jsonBufferLed;
  JsonObject& rootLed = jsonBufferLed.parseObject(s);
  if (rootLed == JsonObject::invalid())
    return;

  Serial.println("Received light from nodemcu: ");
  rootLed.prettyPrintTo(Serial);
  Serial.print("Light ");
  int light = rootLed["light"];
  Serial.print(light);
  return light;
}

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> // http web access library
#include <ArduinoJson.h> // JSON decoding library
// Libraries for SSD1306 OLED display
#include <Wire.h> // include wire library (for I2C devices such as the SSD1306 display)
//#include <Adafruit_GFX.h> // include Adafruit graphics library
//#include <Adafruit_SSD1306.h> // include Adafruit SSD1306 OLED display driver



              //############ LINES 58 to 65 ############//
// set Wi-Fi SSID and password                        //##//
const char *ssid = "XXXXXXXXXX";                      //##//
const char *password = "XXXXXXXXXX";                  //##//
                                                      //##//
// set location and API key                           //##//
String Location = "XXXXXXXXXX, US";                   //##//
String API_Key = "XXXXXXXXXX";                        //##//
              //############ LINES 58 to 65 ############//
void setup(void)


{             //############ LINE 188 ############//
Serial.begin(115200);
delay(1000);

             //############ LINES 193 TO 199  ############//
WiFi.begin(ssid, password);                           //##//
                                                      //##//
Serial.print("Connecting.");                          //##//
while ( WiFi.status() != WL_CONNECTED )               //##//
{                                                     //##//
delay(500);                                           //##//
Serial.print(".");                                    //##//
}                                                     //##//
              //############ LINES 193 TO 199 ############//
Serial.println("connected");
delay(1000);

}

void loop()
{

  
              //############ FETCH() (337 TO 384 ############//
if (WiFi.status() == WL_CONNECTED) //Check WiFi connection status
{
HTTPClient http; //Declare an object of class HTTPClient

// specify request destination
http.begin("http://api.openweathermap.org/data/2.5/weather?q=" + Location + "&APPID=" + API_Key); // !!

int httpCode = http.GET(); // send the request

if (httpCode > 0) // check the returning code
{
String payload = http.getString(); //Get the request response payload

DynamicJsonBuffer jsonBuffer(512);

// Parse JSON object
JsonObject& root = jsonBuffer.parseObject(payload);
if (!root.success()) {
Serial.println(F("Parsing failed!"));
return;
}

float tempC = (float)(root["main"]["temp"]) - 273.15; // get temperature in °C
float tempF = (float)(root["main"]["temp"]) * 9 / 5 - 459.67; // get temperature in °F
int humidity = root["main"]["humidity"]; // get humidity in %
float pressure = (float)(root["main"]["pressure"]) / 1000; // get pressure in bar
float wind_speed = (float)(root["wind"]["speed"]) * 2.237; // get wind speed in m/s
int wind_degree = root["wind"]["deg"]; // get wind degree in °

// print data
Serial.printf("Temperature = %.1f°C\r\n", tempC);
Serial.printf("Temperature = %.1f°F\r\n", tempF);
Serial.printf("Humidity = %d %%\r\n", humidity);
//Serial.printf("Pressure = %.3f bar\r\n", pressure);
Serial.printf("Wind speed = %.1f MPH\r\n\r\n", wind_speed);
//Serial.printf("Wind degree = %d°\r\n\r\n", wind_degree);

}

http.end(); //Close connection

}

delay(60000); // wait 1 minute

}
// End of code.

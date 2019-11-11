
// Header file includes
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include <TimeLib.h> 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


#include <Ticker.h>

Ticker ticker;

#include <ESP8266HTTPClient.h> // http web access library
#include <ArduinoJson.h> // JSON decoding library
///#include "Font_Data.h"

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
#define MAX_DEVICES 8
#define CLK_PIN   14    //D5  BLUE
#define DATA_PIN  13    //D7  GREY
#define CS_PIN    0     //D3  PURPLE


#define PRINT(x)
#define PRINTS(x)
#define PRINTX(x)

//// SPI hardware interface
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
//

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define SPEED_TIME  75
#define PAUSE_TIME  0

#define MAX_MESG  20


// Global variables
char szTime[9];    // mm:ss\0
char szMesg[MAX_MESG+1] = "";

uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 }; // Deg C
uint8_t degF[] = { 6, 3, 3, 124, 20, 20, 4 }; // Deg F


///==================================================================

const char ssid[] = "XXXXXXXXXXXXX";  //  your network SSID (name)
const char pass[] = "XXXXXXXXXXXXX";       // your network password


// set location and API key
String Location = "XXXXXXXXXXXXX, US";
String API_Key = "XXXXXXXXXXXXX";

float tempC ;
float tempF ;
int humidity ;
//float pressure ;
float wind_speed ;
//int wind_degree ;


float tempCs ;
float tempFn ;
int humidity1 ;


 IPAddress timeServer(132, 163 ,97, 1 ); // WORKS


const int timeZone = -8;     // XXXXXXXXXXXXX Time
const int timeZoneZ = 0;     // UTC/Zulu

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets


void getDateZ(char *psz, bool f = true){

int myHour = hour();
int myMin = minute();
int myMon = month();
int myDay = day();


  Serial.print(myMon);
  Serial.print("/");
  Serial.print(myDay);
  Serial.println(); 
//  Serial.print(".");
//  Serial.print(year()); 
  Serial.println(); 
  Serial.println(" ");
  Serial.print(myHour);
  Serial.print(":");
  Serial.print(myMin);
  Serial.println(); 

  
  PRINT(month());
  PRINT("/");
  PRINT(day());
  
}

void maxTime(char *psz, bool f = true){       //LOCAL TIME

int myHour = hour();
int myMin = minute();


  sprintf(psz, "%02d%c%02d", myHour, (f ? ':' : ' '), myMin);

}

void maxTimeZ(char *psz, bool f = true){        // ZULU TIME

int myHourZULU = hour()+8;
int myMinZ = minute();
int myHourZ;




    if ((myHourZULU) > 23.00)  // Adjusting for UTC
    {
      myHourZ=((myHourZULU) - 24);
    }
    else {
      myHourZ=(myHourZULU);
    }

    
  sprintf(psz, "%02d%c%02d", myHourZ, (f ? ':' : ' '), myMinZ);
}



char *mon2str(uint8_t mon, char *psz, uint8_t len)    //MONTH FORMAT
// Get a label from PROGMEM into a char array
{
  static const char str[][4] PROGMEM =
  {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  *psz = '\0';
  mon--;
  if (mon < 12)
  {
    strncpy_P(psz, str[mon], len);
    psz[len] = '\0';
  }

  return(psz);
}


void maxDate(char *psz, bool f = true){       //DATE

  char  szBuf[10];
int myMon = month();
int myDay = day();
  sprintf(psz, "%s %d",mon2str(myMon, szBuf, sizeof(szBuf)-1), myDay);
}



void setup(void)
{

///==================================================================


  Serial.begin(115200);
  while (!Serial) ; // Needed for Leonardo only
  delay(250);
  Serial.println("TimeNTP Example");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);


///==================================================================
  
  P.begin(2);
  P.setInvert(false);

  P.setZone(0, 0, MAX_DEVICES-5);
  P.setZone(1, MAX_DEVICES-4, MAX_DEVICES-1);
////  P.setFont(1, numeric7Seg);

  P.displayZoneText(1, szTime, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(0, szMesg, PA_CENTER, SPEED_TIME, 2500, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  P.addChar('$', degC);
  P.addChar('&', degF);

      fetch();

      

      ticker.attach(8,fetch);      //tickerObj.attach(timeInSecs,isrFunction)

        delay(5000);

}



time_t prevDisplay = 0; // when the digital clock was displayed


void loop(void)
{

///==================================================================


  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }

///==================================================================  
  static uint32_t lastTime = 0; // millis() memory
  static uint8_t  display = 0;  // current display mode
///  static bool flasher = false;  // seconds passing flasher

  P.setIntensity(1);                  //Intensity - Change with time or LDR later
  P.displayAnimate();

  if (P.getZoneStatus(0))
  {
    switch (display)
    {
      case 0:         //ZULU TIME
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_UP_LEFT);
        display++;

        maxTimeZ(szMesg);  

//    delay(5000);        //DELAY NOT WORKING - ZONE DELAY TAKES PRECEDENCE.

        break;

      case 1: // Temperature deg F
        P.setTextEffect(0, PA_SCROLL_UP_LEFT, PA_SCROLL_LEFT);
        display++;


          dtostrf(tempF, 3, 1, szMesg);
          strcat(szMesg, "&");

///        strcpy(szMesg, "71.6&");

        break;

      case 2: // Temperature deg C
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display++;


          dtostrf(tempC, 3, 1, szMesg);
          strcat(szMesg, "$");

        break;

      case 3: // Relative Humidity
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display++;


          dtostrf(humidity, 2, 0, szMesg);
          strcat(szMesg, "%");

        break;

      default:  // Calendar
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        display = 0;
  
        maxDate(szMesg);      

        break;
    }

    P.displayReset(0);
  }

  // Finally, adjust the time string if we have to    //LOCAL TIME
                  if (millis() - lastTime >= 1000)
                  {

        maxTime(szTime);      
                
                
                    P.displayReset(1);
                  }

                  
}


///=======================================================

void fetch(){
    //HOPEFULLY this section will retrieve weather updates.  Hasn't worked yet. :( 
Serial.println("FAIL 1");
//
if (WiFi.status() == WL_CONNECTED) //Check WiFi connection status
{ 
Serial.println("FAIL 2");
HTTPClient http; //Declare an object of class HTTPClient

Serial.println("FAIL 3");
// specify request destination
http.begin("http://api.openweathermap.org/data/2.5/weather?q=" + Location + "&APPID=" + API_Key); // !!

Serial.println("FAIL 4");
int httpCode = http.GET(); // send the request

Serial.println("FAIL 5");
if (httpCode > 0) // check the returning code
{ 
String payload = http.getString(); //Get the request response payload

Serial.println("FAIL 6");
DynamicJsonBuffer jsonBuffer(512);

// Parse JSON object
JsonObject& root = jsonBuffer.parseObject(payload);
if (!root.success()) {
Serial.println(("Parsing failed!"));
return;
}

Serial.println("FAIL 7");

float tempC = (float)(root["main"]["temp"]) - 273.15; // get temperature in °C
float tempF = (float)(root["main"]["temp"]) * 9 / 5 - 459.67; // get temperature in °F
int humidity = root["main"]["humidity"]; // get humidity in %
//float pressure = (float)(root["main"]["pressure"]) / 1000; // get pressure in bar
//float wind_speed = (float)(root["wind"]["speed"]) * 2.237; // get wind speed in m/s
//int wind_degree = root["wind"]["deg"]; // get wind degree in °



Serial.println("FAIL 8");

// print data
Serial.printf("Temperature = %.2f°C\r\n", tempC);
Serial.printf("Temperature = %.2f°F\r\n", tempF);
Serial.printf("Humidity = %d %%\r\n", humidity);
//Serial.printf("Pressure = %.3f bar\r\n", pressure);
//Serial.printf("Wind speed = %.1f MPH\r\n", wind_speed);
//Serial.printf("Wind degree = %d°\r\n\r\n", wind_degree);

        }

http.end(); //Close connection

Serial.println("FAIL 9");
      }

Serial.println("FAIL 10");

  }



void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
//  printDigits(second());
  Serial.println(); 
}


void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

///=======================================================

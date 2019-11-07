# ESP8266-with-MAX7219-Display
ESP8266 with MAX7219 Display - 



I've cobbled this together from many different sources so forgive the extreme mess.  I'm 
STILL trying to learn coding but it's not easy for me.  I've been trying for a very long 
time.  The first problem I'm having is with the weather.  It will fetch the data when it 
starts running and display it in the serial monitor.  It's supposed to get updated data 
every 15 seconds (only for testing.  That'll be 5-10 minutes once it's done) but it's not 
displaying any info after the first time.  
I added "Serial.println("Fetching Weather...");" to see if the interrupt was running at 
all.  I does display that but it's not showing the weather data.


Starting UDP
Local port: 8888
waiting for sync
Transmit NTP Request
Receive NTP Response
Fetching Weather...
This is working
Temperature = 23.46°C
Temperature = 74.23°F
Humidity = 14 %
Pressure = 1.015 bar
Wind speed = 4.7 MPH
Wind degree = 310°

14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
14:33
Fetching Weather...				//THIS SHOULD SHOW NEW WEATHER DATA ALSO
14:33						//It just skips over it.
14:33
14:33


Once that's resolved, all that's left is getting the weather data to display in the zones.  
Lines 267 to 278 for Fahrenheit and 280 to 294 for Celsius and humidity.  I don't think what 
I have there is working but, since I don't know what's going on with the weather data, I 
can't really tell.  It just displays "0.0°F".

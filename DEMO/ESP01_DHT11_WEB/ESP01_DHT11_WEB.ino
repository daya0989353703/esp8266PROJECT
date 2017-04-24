//ESP8266帶DHT11傳感器的溫濕度網絡服務器http://www.elec-cafe.com/esp8266-temperature-humidity-webserver-with-a-dht11-sensor/
/*零件清單:ESP8266 ESP-01
           DHT11溫濕度傳感器
           4.7k電阻
*/
#include <ESP8266WiFi.h>
#include <DHT11.h>

const char* ssid     = "Buffalo"; // Your ssid
const char* password = "1a2b3c4d5e"; // Your Password

int pin = 2;

WiFiServer server(80);
DHT11 dht11(pin);

double Fahrenheit(double celsius) {
return ((double)(9 / 5) * celsius) + 32;
}

double Kelvin(double celsius) {
return celsius + 273.15;
}

void setup() {
Serial.begin(115200);
delay(10);
Serial.println();

// Connect to WiFi network
WiFi.mode(WIFI_STA);
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");

// Start the server
server.begin();
Serial.println("Server started");

// Print the IP address
Serial.println(WiFi.localIP());
}

void loop() {
int err;
float temp, humi;
if ((err = dht11.read(humi, temp)) == 0)
{
Serial.print("temperature:");
Serial.print(temp);
Serial.print(" humidity:");
Serial.print(humi);
Serial.println();
}
else
{
Serial.println();
Serial.print("Error No :");
Serial.print(err);
Serial.println();
}
WiFiClient client = server.available();
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");  // the connection will be closed after completion of the response
client.println("Refresh: 5");  // refresh the page automatically every 5 sec
client.println();
client.println("<!DOCTYPE html>");
client.println("<html xmlns='http://www.w3.org/1999/xhtml'>");
client.println("<head>\n<meta charset='UTF-8'>");
client.println("<title>ESP8266 Temperature & Humidity DHT11 Sensor</title>");
client.println("</head>\n<body>");
client.println("<H2>ESP8266 & DHT11 Sensor</H2>");
client.println("<H3>Humidity / Temperature</H3>");
client.println("<pre>");
client.print("Humidity (%)         : ");
client.println((float)humi, 2);
client.print("Temperature (°C)  : ");
client.println((float)temp, 2);
client.print("Temperature (°F)  : ");
client.println(Fahrenheit(temp), 2);
client.print("Temperature (°K)  : ");
client.println(Kelvin(temp), 2);
client.println("</pre>");
client.println("<H3>www.elec-cafe.com</H3>");
client.print("</body>\n</html>");
delay(DHT11_RETRY_DELAY); //delay for reread
}

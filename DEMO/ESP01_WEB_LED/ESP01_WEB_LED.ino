/*使用 ESP8266, ESP-01 開啟 mDNS 服務，使用者可以使用網頁 ( http://esp8266.local/, http://[esp8266_ip] ) 
  或是網址輸入的方式控制二通道光隔離繼電器模組所連接的燈泡開啟或是關閉。
  http://ruten-proteus.blogspot.tw/2015/09/esp8266-kits-support-arduino-ide.html
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define GPIO0 0
#define GPIO2 2
#define OFF   HIGH
#define ON    LOW

const char* ssid     = "路由器名稱";
const char* password = "路由器連線密碼";

MDNSResponder mdns;

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(80);
void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Connect to WiFi network
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

  // set up mDNS responder:
  if( mdns.begin("esp8266", WiFi.localIP() ) )  // http://esp8266.local/
  {
    Serial.println( "MDNS responder started" );
  }

  server.on( "/", webroot );  // http://esp8266local/
  server.on( "/gpio00", [] {  // GPIO0 OFF, http://esp8266.local/gpio00
    digitalWrite( GPIO0, OFF );
    webroot();
  });
  server.on( "/gpio01", [] {  // GPIO0 ON,  http://esp8266.local/gpio01
    digitalWrite( GPIO0, ON );
    webroot();
  });
  server.on( "/gpio20", [] {  // GPIO2 OFF, http://esp8266.local/gpio20
    digitalWrite( GPIO2, OFF );
    webroot();
  });
  server.on( "/gpio21", [] {  // GPIO2 ON, http://esp8266.local/gpio21
    digitalWrite( GPIO2, ON );
    webroot();
  });

  // GPIO init
  pinMode( GPIO0, OUTPUT );
  pinMode( GPIO2, OUTPUT );
  digitalWrite( GPIO0, OFF );
  digitalWrite( GPIO2, OFF );

  Serial.println( "HTTP Server Started" );
}
// Lamp ON 照片位址
const char* lampON  = "bit.ly/1L5FRD9";
// Lamp OFF 照片位址
const char* lampOFF = "bit.ly/1WigGPz";
// GPIO 按鈕字串
const char* strGpio01 = "<a href=\"/gpio01\"><button>Turn ON</button></a>";
const char* strGpio00 = "<a href=\"/gpio00\"><button>Turn OFF</button></a>";
const char* strGpio21 = "<a href=\"/gpio21\"><button>Turn ON</button></a>";
const char* strGpio20 = "<a href=\"/gpio20\"><button>Turn OFF</button></a>";

void webroot()
{
  // print GPIO setting page
  char temp[2048];

  snprintf( temp, 2048,
"<html>\
<head>\
<meta http-equiv=\"content-type\">\
<title>ESP8266 ESP-01 Arduino IDE Dev.</title>\
</head><body>\
<h1 style=\"text-align: center;\">2-way relay control page<br />\
<span style=\"font-size: xx-small; font-weight: normal; text-align: start;\">\
<i><span style=\"font-size: x-small;\">created by ruten.proteus</span></i></span></h1><br />\
<div style=\"text-align: center;\">\
<img alt=\"Switch OFF\" src=\"https:%c%c%s\" style=\"height: 150px;\" /><br />\
<b><i><u>GPIO0</u></i></b>：&nbsp;&nbsp;\
%s&nbsp;&nbsp;&nbsp;%s&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\
<br /><br />\
<img alt=\"Switch ON\" src=\"https:%c%c%s\" style=\"height: 150px;\" /><br />\
<b><i><u>GPIO2</u></i></b>：&nbsp;&nbsp;\
%s&nbsp;&nbsp;&nbsp;%s&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\
</div></body></html>",
      0x2f, 0x2f,
      ((digitalRead( GPIO0 ))?lampOFF:lampON),  // to choice gpio00 icon
      strGpio01, strGpio00,
      0x2f, 0x2f,
      ((digitalRead( GPIO2 ))?lampOFF:lampON),   // to choice gpio02 icon
      strGpio21, strGpio20
    );
  
  server.send( 200, "text/html;charset=UTF-8", temp );
}

void loop() {
  mdns.update();
  server.handleClient();
}


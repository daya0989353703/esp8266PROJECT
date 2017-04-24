//ESP-01+Line打造簡易保全系統http://pm25.tn.edu.tw/xoops/modules/tad_book3/page.php?tbdsn=40
//

#include <ESP8266WiFi.h>
 
#define SSID    "無線網路SSID"                     
#define PASS    "無線網路密碼"     
 
void setup()
{
  pinMode(2,INPUT);
  WiFi.begin( SSID, PASS );
 }
  
void loop(){
if(digitalRead(2)==HIGH){
WiFiClientSecure client;
     if( !client.connect("maker.ifttt.com", 443 ) )
    {
         return;
    }
    else
    {                                                                //第二組感應器就把value1=1改成value1=2
        String getStr_line = "GET /trigger/sensor_pir/with/key/本段文字替換成您的APIKEY?value1=1  HTTP/1.1\r\n";
        client.print( getStr_line );
        client.print( "Host: maker.ifttt.com\n" );
        client.print( "Connection: close\r\n\r\n" );
        client.stop();
      }  
  
  delay(5000);
}  
}

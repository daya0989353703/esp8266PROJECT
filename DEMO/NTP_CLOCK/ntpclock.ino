//來自http://pm25.tn.edu.tw/xoops/modules/tad_book3/page.php?tbdsn=41
//參考:https://zh.wikipedia.org/wiki/%E7%B6%B2%E8%B7%AF%E6%99%82%E9%96%93%E5%8D%94%E5%AE%9A
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Check I2C address of LCD, normally 0x27 or 0x3F

char ssid[] = "Buffalo";  //  your network SSID (name)
char pass[] = "1a2b3c4d5e";       // your network password

unsigned int localPort = 2390;      // 本地端口監聽UDP報文

IPAddress timeServerIP; // time.nist.gov NTP服務器地址
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP時間戳是消息的前48個字節

byte packetBuffer[ NTP_PACKET_SIZE]; //緩衝區來保存傳入和傳出數據包

// 一個UDP實例，讓我們通過UDP發送和接收數據包
WiFiUDP udp;

void setup()
{
  lcd.begin(0, 2);     // In ESP8266-01, SDA=0, SCL=2
  lcd.backlight();
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
}

void loop()
{
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // 發送NTP報文到時間服務器
  // 等待回復是否可用
  delay(1000);

  udp.parsePacket();
  // 我們收到一個數據包，讀取數據
  udp.read(packetBuffer, NTP_PACKET_SIZE); // 將數據包讀入緩衝區

  //時間戳從接收到的數據包的字節40開始，是四個字節，
   //或兩個字，長。 首先，提出兩個詞：

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // 將四個字節（兩個字）組合成一個長整數
  // 這是NTP時間（1900年1月1日以後的秒數）：
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  //現在將NTP時間轉換成日常時間：
  // Unix時間從1970年1月1日開始。幾秒鐘，這是2208988800：
  const unsigned long seventyYears = 2208988800UL;
  // 減去七十年subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;

  lcd.clear();
  // 打印小時，分和秒：
  lcd.print("UTC+8 time is ");       // UTC是格林威治標準時間（GMT）
  Serial.print("UTC+8 time is ");
  lcd.setCursor(0, 1);
  lcd.print(((epoch  % 86400L ) / 3600) + 8 ); // 打印小時（86400等於每天的秒數）+8為台灣時間
  Serial.print(((epoch  % 86400L ) / 3600) + 8 );
  lcd.print(':');
  Serial.print(':');
  if ( ((epoch % 3600) / 60) < 10 ) {
    // 在每個小時的前10分鐘，我們將要一個預先打印一個'0'
    lcd.print('0');
    Serial.print('0');
  }
  lcd.print((epoch  % 3600) / 60); // 打印分鐘（3600等於每分鐘秒數）
  Serial.print((epoch  % 3600) / 60);
  lcd.print(':');
  Serial.print(':');
  if ( (epoch % 60) < 10 ) {
    // 在每分鐘的前10秒鐘，我們將要一個預先打印一個'0'
    lcd.print('0');
     Serial.print('0');
  }
  lcd.print(epoch % 60); // print the second
 Serial.println(epoch % 60);
  // 等待十秒鐘再要求一次
  delay(9000);
}

// 發送NTP請求給給定地址的時間服務器
unsigned long sendNTPpacket(IPAddress& address)
{
  //Serial.println("sending NTP packet...");
  // 將緩衝區中的所有字節設置為0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // 初始化形成NTP請求所需的值
  // （有關數據包的詳細信息，請參閱上面的URL）
  packetBuffer[0] = 0b11100011;   // LI，版本，模式
  packetBuffer[1] = 0;     // 層或時鐘類型
  packetBuffer[2] = 6;     // 輪詢時間間隔Polling Interval
  packetBuffer[3] = 0xEC;  // 對等鐘精度Peer Clock Precision
  // 根延遲和根分散的8字節零  8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //現在，所有NTP字段都被賦予了值
  // 您可以發送一個請求時間戳的數據包：
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

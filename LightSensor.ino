#define INIT_ADDRES_CELL 1023 // номер резервной ячейки в EEPROM
#define INIT_EEPROM_KEY 50  // ключ первого запуска. 0-254, на выбор
#include <EEPROM.h>
/* Create a WiFi access point and provide a web server on it. */
#include <BH1750FVI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#ifndef APSSID
#define APSSID "ESPap"
#define APPSK "11111111"
#endif
uint16_t lux = 0;
uint16_t luxOn = 150;
uint16_t luxOnRead = 0;
uint16_t luxOff = 0;
String onOff = "";

String host = "host";
String ssName = "ssName";
int ss =150;
int pw = 170;
// String webPage = "" ;

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);
// Create the Lightsensor instance
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  EEPROM.begin(4096);  // для esp8266/esp32
  if (EEPROM.read(INIT_ADDRES_CELL) != INIT_EEPROM_KEY) { // первый запуск
    EEPROM.write(INIT_ADDRES_CELL, INIT_EEPROM_KEY); // записали ключ
    Serial.println("First start EEPROM!!");
    EEPROM.commit();    // для esp8266/esp32
    // записали стандартное значение яркости
    // в данном случае это значение переменной, объявленное выше
    EEPROM.put(0, luxOn);
    EEPROM.commit();     // для esp8266/esp32
  }
  pinMode(14, OUTPUT);  // GPIO14 как выход
  // delay(1000);
  
  Serial.println();
  EEPROM.get(0, luxOnRead);
  Serial.println("LuxOn by EEPROM = " + String(luxOnRead));
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/1", handleRoot1);
    server.on("/2", handleRoot2);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
  // put your setup code here, to run once:
  LightSensor.begin();
}

void loop() {
  server.handleClient();

  // put your main code here, to run repeatedly:
  lux = LightSensor.GetLightIntensity();
  Serial.print("Light: ");
  Serial.println(lux);
  if (lux < luxOn) {
    digitalWrite(14, LOW);
    onOff = "ON";
  } else {
    digitalWrite(14, HIGH);
    onOff = "OFF";
  }
  delay(1000);
}

//  Just a little test message.  Go to http://192.168.4.1 in a web browser
//    connected to this access point to see it.
//   В этой функции чтобы получить число в строковом виде нужно к инту подставлять преобразователь стринг!!!
//  Тогда все работает.
void handleRoot() {
  server.send(200, "text/html", "<h1>Light: " + String(lux) + "</h1>");  //"<h1>You are connected</h1>" + String(stringOne)
}

void handle_NotFound() {
  server.send(404, "text/plain", "Page not found");
}

void handleRoot1() {
  server.send(200, "text/html", SendHTML(lux, luxOn, luxOff, onOff));  //"<h1>You are connected</h1>" + String(stringOne)
}



String SendHTML(uint16_t lux, uint16_t luxOn, uint16_t luxOff, String onOff) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  // ptr += <meta http-equiv="refresh" content="2">\n;
  ptr += "<title>Light control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 20px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Light sensor Web Server</h1>\n";
  ptr += "<h3>Using Access Point(AP) Mode</h3>\n";

  ptr += "<h3> The light is: " + String(onOff) + "</h3>\n";

  // if(lux != 0)
  ptr += "<p> On light sensor data: " + String(luxOn) + " lux</p>\n";
  ptr += "<p> Current sensor data: " + String(lux) + " lux</p>\n";

  ptr += "<p> Off light sensor data: " + String(luxOff) + " lux</p>\n";
  // else
  // ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";

  // if(led2stat)
  //   ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";
  // else
  //   ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void handleRoot2() {
  server.send(200, "text/html", Send(host, ssName, ss, pw));  
}

String Send( String host, String ssName,  int ss, int pw) {
    
    String webPage = "<!DOCTYPE html><html><body>";
    webPage += "<h2>Settings Form</h2>";
    webPage += "<form action=\"/ok\"";
    webPage += "<label for=\"light_start\">Lighting start:</label><br>";
    webPage += "<input type=\"text\" id=\"light_start\" name=\"light_start\" placeholder=" + String(ss) + "><br>";

    webPage += "<label for=\"light_finish\">Lighting finish:</label><br>";
    webPage += "<input type=\"text\" id=\"light_finish\" name=\"light_finish\" placeholder=" + String(pw) + "><br><br>";

    webPage += "<input type = \"submit\" value = \"Accept parametrs\">";
    webPage += "</form></body></html>";
    
    return webPage;
}

// String Send( String host, String ssName,  int ss, int pw) {
//     //фишка в том что все запрашиваемые данные должны находиться внитри одной <form>
//     //допилить внешний вид
    
//     // webPage = "" ;
//     String webPage = "<!DOCTYPE html><html><body><form action=\"/ok\"" ;
//     webPage += "Server IP, now is: " + host + "<br>" ;
//     webPage += "<input type = \"text\" name =\"ip\" ";
//     webPage += "Sensor Name, now is: " + String(ssName) + "<br>" ;
//     webPage += "<input type = \"text\" name =\"SN\" ";
//     webPage +=     "SSID now is: " + String(ss) + "<br>" ;
//     webPage += "<input type = \"text\" name =\"SSID\" ";
//     webPage += "wifi password now is: " + String(pw) + "<br>" ;
//     webPage += "<input type = \"text\" name =\"passwd\"<br>" ;
//     webPage += "<input type = \"submit\" value = \"Accept parametrs\">" ;
//     webPage += "</form></body></html>" ;
//     return webPage;
// }

 
    

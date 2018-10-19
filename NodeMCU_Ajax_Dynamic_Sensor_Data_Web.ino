#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASS";
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

String page = "";
String text = "";
double data = 0;

unsigned long currentTime;
unsigned long lastMillis = 0;

void setup(void) {
  pinMode(A0, INPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/data.txt", []() {
    text = (String)data;
    server.send(200, "text/html", text);    //Stores a text file with the sensor reading which is later used to update the value on the web page.
  });
  server.on("/", []() {
    page = "<!DOCTYPE html><html>";
    page += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    page += "<link rel=\"icon\" href=\"data:,\">";
    page += "<style>body, html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; height: 100%; width: 100%; overflow: hidden; }";
    page += "* { box-sizing: border-box; }";
    page += ".bg-image {background-image: url(\"https://upload.wikimedia.org/wikipedia/commons/thumb/d/d6/Hs-2009-25-e-full_jpg.jpg/800px-Hs-2009-25-e-full_jpg.jpg\"); filter: blur(8px); -webkit-filter: blur(8px); height: 100%; background-position: center; background-repeat: no-repeat; background-size: cover; }";
    page += ".bg-text { background-color: rgb(0,0,0); background-color: rgba(0,0,0, 0.4); color: white; font-weight: bold; position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); z-index: 2; width: 80%; padding: 20px; text-align: center; }</style></head>";
    page += "<body><h1>Sensor to Node MCU Web Server</h1><div class=\"bg-image\"></div>";
    page += "<div class=\"bg-text\"><h2>Inside Dimi's House</h2>";
    page += "<h3>Temperature:</h3><h3 id=\"data\">""</h3></div>\r\n";
    page += "<script>\r\n";
    page += "var x = setInterval(function() {loadData(\"data.txt\",updateData)}, 2000);\r\n";
    page += "function loadData(url, callback){\r\n";
    page += "var xhttp = new XMLHttpRequest();\r\n";
    page += "xhttp.onreadystatechange = function(){\r\n";
    page += " if(this.readyState == 4 && this.status == 200){\r\n";
    page += " callback.apply(xhttp);\r\n";
    page += " }\r\n";
    page += "};\r\n";
    page += "xhttp.open(\"GET\", url, true);\r\n";
    page += "xhttp.send();\r\n";
    page += "}\r\n";
    page += "function updateData(){\r\n";
    page += " document.getElementById(\"data\").innerHTML = this.responseText;\r\n";
    page += "}\r\n";
    page += "</script></body></html>\r\n";
    server.send(200, "text/html", page);
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop(void) {
  currentTime = millis();
  if (currentTime - lastMillis > 2000) {
    lastMillis = currentTime;
    data = getTemperature();
  }
  server.handleClient();
}

double getTemperature() {
  int reading = analogRead(A0);
  float voltage = (reading * 3.3) / 1023.0;
  float temp = 7 / (voltage - 0.64);
  return temp;
}


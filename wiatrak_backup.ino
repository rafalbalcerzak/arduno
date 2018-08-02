#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "WiFi63";
const char* password = "980130@WiFi";

ESP8266WebServer server(80);

const int led = 13;
#define LED_BUILTIN 2
int stan = 0;
int blinking = 0;
int time5min = 0;
int relay = 2;

unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(relay, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  

  server.on("/", handleRoot);

   server.on("/blink", [](){
     blinking = 1;
     time5min = 0;
    server.send(200, "text/plain", "blink");
   });

  server.on("/5min", [](){
    stan = 0;
    time5min = 1;
    blinking = 0;
    server.send(200, "text/plain", "5 min");
   });
   
  server.on("/on", [](){
    blinking = 0;
    time5min = 0;
    digitalWrite(relay, LOW);
    server.send(200, "text/plain", "on");
  });

  server.on("/off", [](){
    blinking = 0;
    time5min = 0;
    digitalWrite(relay, HIGH);
    server.send(200, "text/plain", "off");
  });
   
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();

  if(time5min == 1){
       aktualnyCzas = millis();
       roznicaCzasu = aktualnyCzas - zapamietanyCzas; 
       
       if (roznicaCzasu >= 10000UL) {
        //Zapamietaj aktualny czas
        zapamietanyCzas = aktualnyCzas;
        //Wyslij do PC
        if (stan == 0){
          stan = 1;
          digitalWrite(relay, LOW);
          digitalWrite(LED_BUILTIN, LOW);
          }
        else if (stan == 1){
          stan = 2;
          digitalWrite(relay, HIGH);
          digitalWrite(LED_BUILTIN, HIGH);
        }
        else{
          
        }
    }
    }

  
  if(blinking == 1){
    aktualnyCzas = millis();
     roznicaCzasu = aktualnyCzas - zapamietanyCzas;
  
      //Jeśli różnica wynosi ponad sekundę
     if (roznicaCzasu >= 1000UL) {
      //Zapamietaj aktualny czas
      zapamietanyCzas = aktualnyCzas;
      //Wyslij do PC
      if (stan == 0){
        stan = 1;
        digitalWrite(relay, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);
        }
      else{
        stan = 0;
        digitalWrite(relay, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
      }
  }
  }
}

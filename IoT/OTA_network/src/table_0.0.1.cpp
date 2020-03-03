#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

char mSSID[] = "Note8"; // to set as required
char mPASS[] = "12345srs"; // to set as required

//// WHERE TO FIND NEW SKETCH
String Cliente = "table";
String NewVersion = "0.0.1"; // NEW VERSION OF THE CODE → if the current version is, for example, 0.0.3 and in the remote server there´s a version 0.0.4, it will update
String url_server = "http://132.248.51.251/datos/"+Cliente; // FOLDER NAME → if use a folder with the client name
String url_sketch = url_server+"/"+Cliente+"_"+NewVersion+".bin"; // new URL → it's builds the URL to get the compiled code file (.bin) - I used a file with the client's name plus new version number
bool Continue = true;

void setup() {
  Serial.begin(9600);
  Serial.println(url_sketch);
  Serial.print("Connecting to ");
  Serial.println(mSSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(mSSID, mPASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conected");
}

//// UPDATING SKETCH ROUTINE
void OTA_Updates(){
 Serial.println(Cliente);
 Serial.println(NewVersion);
 Serial.println(url_server);
 Serial.println(url_sketch);
 if((WiFi.status() == WL_CONNECTED)) {
      t_httpUpdate_return ret = ESPhttpUpdate.update("132.248.51.251", 80, "/datos/table_0.0.1.bin");
      switch(ret) {
              case HTTP_UPDATE_FAILED:
              Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n",
              ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
              break;

              case HTTP_UPDATE_NO_UPDATES:
              Serial.println("HTTP_UPDATE_NO_UPDATES");
              break;

              case HTTP_UPDATE_OK:
              Serial.println("HTTP_UPDATE_OK");
              break;
        }
        Continue = false;
  }
}

void loop() {
if (Continue){
  if(Serial.available()){
    Serial.println("Buscar archivo");
    OTA_Updates();
  }
  }
}

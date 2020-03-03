#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <Wire.h>
int address = 1;
int value;
int numSize;

char ssid[] = "CMI_wireless";     // el nombre de la Red
char password[] = "r0b0tk5k@"; // la clave de la Red

// Initialize Telegram BOT
#define BOTtoken "669762654:AAE8hIf--7MJdnW7E9D8SbDMDefrMPBFQk4"  // el token de tu BOT, lo obtenemos de BotFather

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 20; //Tiempo medio entre el escane0 de omensajes
long Bot_lasttime = 0;   //exploracion de el ultimo mensaje

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;
    numSize = text.length();
    value = text.toInt();
    Serial.println(value);
  }

  Wire.beginTransmission(0); 
  Wire.write("N");        
  Wire.endTransmission();
  
  for (int j = 0; j<numSize; j++){
    int number = value%10;
    value = value/10;
    if(number == 0){
      Wire.beginTransmission(address); 
      Wire.write("N123456");        
      Wire.endTransmission();  
    }
    else if(number == 1){
      Wire.beginTransmission(address); 
      Wire.write("N23");        
      Wire.endTransmission();
    }
    else if(number == 2){
      Wire.beginTransmission(address); 
      Wire.write("N12754");       
      Wire.endTransmission(); 
    }
    else if(number == 3){
      Wire.beginTransmission(address); 
      Wire.write("N12347");        
      Wire.endTransmission();      
    }
    else if(number == 4){
      Wire.beginTransmission(address); 
      Wire.write("N2367");        
      Wire.endTransmission();     
    }
    else if(number == 5){
      Wire.beginTransmission(address); 
      Wire.write("N13467");        
      Wire.endTransmission();   
    }
    else if(number == 6){
      Wire.beginTransmission(address); 
      Wire.write("N134567");        
      Wire.endTransmission();     
    }
    else if(number == 7){
      Wire.beginTransmission(address); 
      Wire.write("N123");        
      Wire.endTransmission();    
    }
    else if(number == 8){
      Wire.beginTransmission(address); 
      Wire.write("N31234567");        
      Wire.endTransmission();    
    }
    else if(number == 9){
      Wire.beginTransmission(address); 
      Wire.write("N12367");        
      Wire.endTransmission();    
    }
    address++;
  }
}


void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  
  // Establecer el modo WiFi y desconectarse de un AP si fue Anteriormente conectada
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  // Intentar conectarse a la red
  Serial.print("Conectando al Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi conectada");
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      address = 1;
    }

    Bot_lasttime = millis();
  }
}

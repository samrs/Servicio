#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <EEPROM.h>

//enable only one of these below.
//#define CHECK_CA_ROOT
//#define CHECK_PUB_KEY
#define CHECK_FINGERPRINT

#include "secrets.h"

#ifndef SECRET
    #define ssidSize 
    #define passSize 
    #define topicSize
    #define hostSize
    #define userSize
    #define passSessionSize

    #define hostName "dispositivo host"

    const char ssid[] = "WiFiSSID";
    const char password[] = "Password";

    const char mqtt_host[] = "000.000.0.00";
    const int mqtt_port = 0000;
    const char mqtt_user[] = ""; 
    const char mqtt_pass[] = "";
    const char sub_topic[] = "topic";
    const char pub_topic[] = "topic";

    #ifdef CHECK_CA_ROOT
    static const char digicert[] PROGMEM = R"EOF(
    -----BEGIN CERTIFICATE-----
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    -----END CERTIFICATE-----

    )EOF";
    #endif

    #ifdef CHECK_PUB_KEY
    // Extracted by: openssl x509 -pubkey -noout -in ca.crt
    static const char pubkey[] PROGMEM = R"KEY(
    -----BEGIN PUBLIC KEY-----
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    -----END PUBLIC KEY-----

    )KEY";
    #endif

    #ifdef CHECK_FINGERPRINT
	// Extracted by: openssl x509 -fingerprint -in ca.crt
    static const char fp[] PROGMEM = "AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD";
    #endif
#endif

#if (defined(CHECK_PUB_KEY) and defined(CHECK_CA_ROOT)) or (defined(CHECK_PUB_KEY) and defined(CHECK_FINGERPRINT)) or (defined(CHECK_FINGERPRINT) and defined(CHECK_CA_ROOT)) or (defined(CHECK_PUB_KEY) and defined(CHECK_CA_ROOT) and defined(CHECK_FINGERPRINT))
  #error "cant have both CHECK_CA_ROOT and CHECK_PUB_KEY enabled"
#endif

int address = 1;
int number;
char warning;
int addr = 0;
int counter_wrong_keys = 0;

char ssid_2[ssidSize];
char password_2[passSize];
char principalTopic[topicSize];
char unsubscribe[topicSize];
char mqtt_host_2[hostSize];
char mqtt_user_2[userSize]; 
char mqtt_pass_2[passSessionSize];

bool ssidLoad = false;
bool passwordLoad = false;
bool mqttUser2Load = false; 
bool mqttPass2Load = false;

BearSSL::WiFiClientSecure espClient;
PubSubClient client(espClient);

void certificate(){
  #ifdef CHECK_CA_ROOT
    BearSSL::X509List cert(digicert);
    espClient.setTrustAnchors(&cert);
  #endif
  #ifdef CHECK_PUB_KEY
    BearSSL::PublicKey key(pubkey);
    espClient.setKnownKey(&key);
  #endif
  #ifdef CHECK_FINGERPRINT
    espClient.setFingerprint(fp);
  #endif
  #if (!defined(CHECK_PUB_KEY) and !defined(CHECK_CA_ROOT) and !defined(CHECK_FINGERPRINT))
    espClient.setInsecure();
  #endif
}

void setup_wifi(const char* ssidConnect, const char* passwordConnect) {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssidConnect);
  WiFi.hostname(hostName);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssidConnect, passwordConnect);

  while (WiFi.status() != WL_CONNECTED) {
    counter_wrong_keys ++;
    delay(5000);
    Serial.print(".");
    if(counter_wrong_keys == 6){
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(ssid);
    }
  }
  counter_wrong_keys = 0;
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttConnect(const char* user, const char* password) {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(hostName, user, password)){
      Serial.println("connected");
      client.subscribe(sub_topic);
      if(strlen(principalTopic) != 0){
        client.subscribe(principalTopic);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      counter_wrong_keys++;
      if (counter_wrong_keys == 6){
        client.setServer(mqtt_host, mqtt_port);
        mqttConnect(mqtt_user,mqtt_pass);
      }
    }
  }
  counter_wrong_keys = 0;
}

void eepromWrite(int maxSize, int len, char* values) {
  for (int i = 0; i < len; i++) {
    EEPROM.write(addr+i, values[i]);
  }
  for (int i = len; i < maxSize; i++) {
    EEPROM.write(addr+i, 255);
  }
  EEPROM.commit();
}

String eepromRead(int maxSize) {
   byte red;
   String strRead;
   for (int i = addr; i < addr+maxSize; i++) {
      red = EEPROM.read(i);
      if (red != 255) {
        strRead += (char)red;
      }
   }
   addr+=maxSize;
   return strRead;
}

void saveKeys(char* save, byte* payload, unsigned int length, int maxSize){
  memset(save,'\0',maxSize);
  for (int j = 0; j < length; j++){
    save[j] = (char)payload[j];
  }
  eepromWrite(maxSize, length, save);
  if(ssidLoad == true && passwordLoad == true){
    WiFi.disconnect();
    setup_wifi(ssid_2, password_2);
    ssidLoad = false;
    passwordLoad = false;
  }
  if(mqttUser2Load == true && mqttPass2Load == true){
    client.disconnect();
    client.setServer(mqtt_host, mqtt_port);
    mqttConnect(mqtt_user_2, mqtt_pass_2);
    mqttUser2Load = false;
    mqttPass2Load = false;
  }
}

void setNumber (int place, char* code){
  Wire.beginTransmission(place);
  Wire.write(code);
  Wire.endTransmission();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  if((String)topic == "CMI/ssid" || (String)topic == "CMI/display/ssid"){
    addr = 0;
    ssidLoad = true;
    saveKeys(ssid_2, payload, length, ssidSize);
    return;
  }
  else if((String)topic == "CMI/password" || (String)topic == "CMI/display/password"){
    addr = ssidSize;
    passwordLoad = true;
    saveKeys(password_2, payload, length, passSize);
    return;
  }
  
  else if((String)topic == "CMI/user" || (String)topic == "CMI/display/user"){
    addr = ssidSize + passSize;
    mqttUser2Load = true;
    saveKeys(mqtt_user_2, payload, length, userSize);
    return;
  }

  else if((String)topic == "CMI/user/password" || (String)topic == "CMI/display/user/password"){
    addr = ssidSize + passSize + userSize;
    mqttPass2Load = true;
    saveKeys(mqtt_pass_2, payload, length, passSessionSize);
    return;
  }

  else if((String)topic == "CMI/host" || (String)topic == "CMI/display/host"){
    addr = ssidSize + passSize + userSize + passSessionSize;
    saveKeys(mqtt_host_2, payload, length, hostSize);
    client.disconnect();
    client.setServer(mqtt_host_2, mqtt_port);
    mqttConnect(mqtt_user, mqtt_pass);
    return;
  }

  else if((String)topic == "CMI/subscribe" || (String)topic == "CMI/display/subscribe"){
    addr = ssidSize + passSize + userSize + passSessionSize + hostSize;
    saveKeys(principalTopic, payload, length, topicSize);
    client.subscribe(principalTopic);
    return;
  }

  else if((String)topic == "CMI/unsubscribe" || (String)topic == "CMI/display/unsubscribe"){
    client.unsubscribe(principalTopic);
    memset(principalTopic,'\0',topicSize);
    return;
  }

  else if((String)topic == principalTopic){
    setNumber (0, "N");
    setNumber (2, "N");
    for (int i = length-1; i > -1; i--) {
      number = ((char)payload[i]) - '0';
      warning = (char)payload[i];
      if(warning == 'N'){
        setNumber (0, "N");
        setNumber (2, "N");
      }
      switch (number)
      {
        case 0:
          setNumber (address, "N123456");
          break;
        case 1:
          setNumber (address, "N23");
          break;
        case 2:
          setNumber (address, "N12754");
          break;
        case 3:
          setNumber (address, "N12347");
          break;
        case 4:
          setNumber (address, "N2367");
          break;
        case 5:
          setNumber (address, "N13467");
          break;
        case 6:
          setNumber (address, "N134567");
          break;
        case 7:
          setNumber (address, "N123");
          break;
        case 8:
          setNumber (address, "N31234567");
          break;
        case 9:
          setNumber (address, "N12367");
          break;
        default:
          break;
      }
      address++;
    }
    address = 1;
    Serial.println();
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin(); //D1-SCL D2-SDA
  EEPROM.begin(512);
  eepromRead(ssidSize).toCharArray(ssid_2, ssidSize);
  eepromRead(passSize).toCharArray(password_2, passSize);
  eepromRead(userSize).toCharArray(mqtt_user_2, userSize);
  eepromRead(passSessionSize).toCharArray(mqtt_pass_2, passSessionSize);
  eepromRead(hostSize).toCharArray(mqtt_host_2, hostSize);
  eepromRead(passSessionSize).toCharArray(principalTopic, passSessionSize);
  Serial.println(ssid_2);
  Serial.println(password_2);
  Serial.println(mqtt_user_2);
  Serial.println(mqtt_pass_2);
  Serial.println(mqtt_host_2);
  Serial.println(principalTopic);
  addr = 0;

  certificate();

  if(strlen(ssid_2) != 0 && strlen(password_2) != 0){
    setup_wifi(ssid_2, password_2);
  }
  else{
    setup_wifi(ssid, password);
  }
  if(strlen(mqtt_host_2) != 0){
    client.setServer(mqtt_host_2, mqtt_port);
  }
  else{
    client.setServer(mqtt_host, mqtt_port);
  }
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    if(strlen(mqtt_user_2) != 0 && strlen(mqtt_pass_2) != 0){
      mqttConnect(mqtt_user_2, mqtt_pass_2);
    }
    else{
      mqttConnect(mqtt_user, mqtt_pass);
    }
  }
  client.loop();
  if(warning == 'a'){
      setNumber (1, "N1234567");
      setNumber (2, "N1234567");
      delay(500);
      setNumber (1, "N");
      setNumber (2, "N");
      delay(500);
    }
}
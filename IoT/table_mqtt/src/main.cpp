#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
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

char ssid_2[ssidSize];
char password_2[passSize];
char principalTopic[topicSize];
char pub_topic_position[220];
char unsubscribe[topicSize];
char mqtt_host_2[hostSize];
char mqtt_user_2[userSize]; 
char mqtt_pass_2[passSessionSize];

bool ssidLoad = false;
bool passwordLoad = false;
bool principalTopicLoad = false;
bool mqttUser2Load = false; 
bool mqttPass2Load = false;
int addr = 0;
int counter_wrong_keys = 0;

const int dirPin = 2;
const int stepPin = 0;
const int stopPin = 13;
const float steps = 2740.0;//2745
float stepsCounter = 0.0;
int stepSpeed=1;
int CW = 1;
bool turnOn = false;

const int channelPinA = 4;
const int channelPinB = 5;
const int stepsEncoder = 1232;//
int posCounter = 0;
const int timeThreshold = 5;
bool IsCW = true;
long timeCounter = 0;

char msg[8];
String option;

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
  //randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttConnect(const char* user, const char* password) {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(hostName, user, password)) {
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
  if (principalTopicLoad){
    save[length++]='/';
    save[length++]='#';
    principalTopicLoad = false;
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  option = topic;

  if(option == "CMI/ssid" || option == "CMI/table/ssid"){
    addr = 0;
    ssidLoad = true;
    saveKeys(ssid_2, payload, length, 32);
    return;
  }
  else if(option == "CMI/password" || option == "CMI/table/password"){
    addr = ssidSize;
    passwordLoad = true;
    saveKeys(password_2, payload, length, 64);
    return;
  }
  else if(option == "CMI/user" || option == "CMI/table/user"){
    addr = ssidSize + passSize;
    mqttUser2Load = true;
    saveKeys(mqtt_user_2, payload, length, userSize);
    return;
  }

  else if(option == "CMI/user/password" || option == "CMI/table/user/password"){
    addr = ssidSize + passSize + userSize;
    mqttPass2Load = true;
    saveKeys(mqtt_pass_2, payload, length, passSessionSize);
    return;
  }
  else if(option == "CMI/host" || option == "CMI/table/host"){
    addr = ssidSize + passSize + userSize + passSessionSize;
    saveKeys(mqtt_host_2, payload, length, hostSize);
    client.disconnect();
    client.setServer(mqtt_host_2, mqtt_port);
    mqttConnect(mqtt_user, mqtt_pass);
    return;
  }
  else if(option == "CMI/subscribe" || option == "CMI/table/subscribe"){
    addr = ssidSize + passSize + userSize + passSessionSize + hostSize;
    principalTopicLoad = true;
    saveKeys(principalTopic, payload, length, topicSize);
    client.subscribe(principalTopic);
    strncpy(pub_topic_position, principalTopic, strlen(principalTopic)-2);
    strcat(pub_topic_position, "/position/value");
    return;
  }
  else if(option == "CMI/unsubscribe" || option == "CMI/table/unsubscribe"){
    client.unsubscribe(principalTopic);
    memset(principalTopic,'\0',topicSize);
    return;
  }

  char selection[220];
  for(int i=0; i<200; i++){
    selection[i] = option[i+strlen(principalTopic)-1];
  }
  option = selection;
  if(option == "speed"){
    stepSpeed = 0;
    for (int i = 0; i < length; i++) {
      stepSpeed += ((char)payload[i]-'0')*pow(10,length-i-1);
    }
    Serial.println(stepSpeed);
  }
  else if(option == "enable"){
    if ((char)payload[0] == '0'){
      digitalWrite(stopPin, LOW);
      turnOn = false;
    }
    else if((char)payload[0] == '1'){
      digitalWrite(stopPin, HIGH);
      turnOn = true;
    }
    Serial.println((char)payload[0]);
  }
  else if(option == "direction"){
    if ((char)payload[0] == '0'){
      CW = 0;
    }
    else if ((char)payload[0] == '1'){
      CW = 1;
    }
  }
  else if(option == "position"){
    float position = (posCounter*1.0)/(stepsEncoder*1.0)*360.0*1.0;
    //Serial.println(position);
    //snprintf (msg, 50, "%ld", position);
    dtostrf(position, 6, 2, msg);
    //Serial.println(msg);
    if(strlen(principalTopic) != 0){
      client.publish(pub_topic_position, msg);
    }
  }
  else if(option == "point"){
    digitalWrite(stopPin, HIGH);
    float angle=0.0;
    int direction;
    for (int i = 0; i < length; i++) {
      angle += ((char)payload[i]-'0')*pow(10,length-i-1);
    }
    //float toPosition = ((angle*1.0/360.0)*(steps*1.0) - stepsCounter*1.0);
    float toDirection = ((angle*1.0/360.0)*(stepsEncoder*1.0) - posCounter*1.0);
    int toPosition = abs((angle*1.0/360.0)*(stepsEncoder*1.0));
    if (toDirection < 0){ direction = 0; }
    else { direction = 1; }
    while (toPosition != posCounter){
      digitalWrite(dirPin, direction);
      digitalWrite(stepPin, HIGH);
      delay(stepSpeed);
      digitalWrite(stepPin, LOW);
      delay(stepSpeed);
      /*
      if (direction == 1){
        stepsCounter++;
        if (stepsCounter > steps){stepsCounter = 0.0;}
      }
      else{
        stepsCounter--;
        if (stepsCounter < 0){stepsCounter = steps;}
      }
      */
    }
    Serial.println(posCounter);
    digitalWrite(stopPin, LOW);
  }
}

void doEncodeA()
{
  if(micros() > timeCounter+timeThreshold){
    if(digitalRead(channelPinA) == digitalRead(channelPinB))
    {
      IsCW = true;
      posCounter++;
      if (posCounter > stepsEncoder){
        posCounter=0.0;
      }
    }
    else
    {
      IsCW = false;
      posCounter--;
      if (posCounter < 0){
        posCounter=stepsEncoder;
      }
    }
    //Serial.println(posCounter);
    timeCounter = micros();
  }
}
void doEncodeB()
{
  if(micros() > timeCounter+timeThreshold){
    if(digitalRead(channelPinA) != digitalRead(channelPinB))
    {
      IsCW = true;
      posCounter++;
      if (posCounter > stepsEncoder){
        posCounter=0;
      }
    }
    else
    {
      IsCW = false;
      posCounter--;
      if (posCounter < 0){
        posCounter=stepsEncoder;
      }
    }
    timeCounter = micros();
  }
}

void setup() {
  Serial.begin(9600);
  system_update_cpu_freq(160);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(stopPin, OUTPUT);
  pinMode(channelPinA, INPUT_PULLUP);
  pinMode(channelPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(channelPinA), doEncodeA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(channelPinB), doEncodeB, CHANGE);
  EEPROM.begin(512);
  eepromRead(ssidSize).toCharArray(ssid_2, ssidSize);
  eepromRead(passSize).toCharArray(password_2, passSize);
  eepromRead(userSize).toCharArray(mqtt_user_2, userSize);
  eepromRead(passSessionSize).toCharArray(mqtt_pass_2, passSessionSize);
  eepromRead(hostSize).toCharArray(mqtt_host_2, hostSize);
  eepromRead(passSessionSize).toCharArray(principalTopic, passSessionSize);
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
  if(strlen(principalTopic) != 0){
    client.subscribe(principalTopic);
    strncpy(pub_topic_position, principalTopic, strlen(principalTopic)-2);
    strcat(pub_topic_position, "/position/value");
    Serial.println(pub_topic_position);
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
  if (turnOn){
    digitalWrite(stepPin, HIGH);
    delay(stepSpeed);
    digitalWrite(stepPin, LOW);
    delay(stepSpeed);
    digitalWrite(dirPin, CW);
    /*
    if (CW == 1){
      stepsCounter++;
      if (stepsCounter > steps){stepsCounter = 0.0;}
    }
    else{
      stepsCounter--;
      if (stepsCounter < 0){stepsCounter = steps;}
    }
    */
  }
}
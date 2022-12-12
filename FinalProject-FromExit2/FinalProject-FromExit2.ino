#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"
#include <Wire.h>
#include "SNIPE.h"

#define ATSerial Serial

#define PING 1
#define PONG 2 
#define CODE PING 

#define TXpin 13
#define RXpin 15

#define trigPin 14
#define echoPin 12

#define AWS_IOT_PUBLISH_TOPIC   "$aws/things/Project_ESP8286/shadow/update/accepted"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266/sub"

LiquidCrystal_I2C lcd(0x3F, 16, 2); // SDA(초록선) -> D4  SCL(하얀선) -> D3 
SoftwareSerial DebugSerial(RXpin, TXpin);
SNIPE SNIPE(ATSerial);

WiFiClientSecure net;

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

PubSubClient client(net);

time_t now;
time_t nowish = 1510592825;

int SR_count =0;
int distance;
int count;

unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  
void NTPConnect(void){
  DebugSerial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    DebugSerial.print(".");
    now = time(nullptr);
  }
  DebugSerial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  DebugSerial.print("Current time: ");
  DebugSerial.print(asctime(&timeinfo));
}

void setup(){
  ATSerial.begin(115200); // 로라 Serial 속도 설정 
  DebugSerial.begin(115200);
  connectAWS();
  
  lcd.begin();
  lcd.backlight();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // put your setup code here, to run once:, Serial 안전하게 초기화 하는 코드
  // 초기에 쓰레기값이 읽혀지는 것을 방지하기 위해 Serial 값이 읽혀지지 않을때까지 읽어서 초기화
  while(ATSerial.read()>= 0) {} 
  while(!ATSerial); 

/*
 * 로라 아두이노 쉴드 초기화 : 아두이노 보드에서 로라 아두이노 쉴드로 초기화 명령을 보내서
 * 로라 쉴드 정상 동작 여부 확인 코드
 */
  if (!SNIPE.lora_init()){
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }

/*
 * Appkey 설정된 값으로 변경 : 성공하면 True, 실패하면 False
 * 노드들은 무조건 같은 Key로 구성한다. 
 */
  if (!SNIPE.lora_setAppKey(lora_app_key)){
    DebugSerial.println("SNIPE LoRa app key value has not been changed");
  }

/*
 * 로라 주파수(채널)설정 : 1번으로 설정해서 True or False 반환
 */
  if (!SNIPE.lora_setFreq(LORA_CH_1)){
    DebugSerial.println("SNIPE LoRa Frequency value has not been changed");
  } 

/*
 * 로라 Spreading Factor(확산요소) 값 설정 : 7번으로 설정해서 True or False 반환 
 * 확산 요소 : 로라 신호를 멀리 보내는데 있어서 가장 중요한 설정값으로, 
 * 7은 짧은시간안에 짧은 거리 / 12는 긴 시간 가장 멀리 (데이터 크기는 갈수록 줄음)
 * 
 */
  if (!SNIPE.lora_setSf(LORA_SF_7)){
    DebugSerial.println("SNIPE LoRa Sf value has not been changed");
  }

/*
 * 로라 TX TimeOut 값 설정 : 4초로 설정 
 * 설정한 시간동안 로라 데이터를 송신  
 */
  if (!SNIPE.lora_setTxtout(4000)){
    DebugSerial.println("SNIPE LoRa Rx Timout value has not been changed");
  }
  DebugSerial.println("SR TEST START!");
}

long microsecondsToCm(long microseconds)
{
  return microseconds / 29 / 2 ;
}

void loop(){
  long duration, cm;
  now = time(nullptr);
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  // echoPin이 High를 유지한 시간을 저장함 
  // 즉 , 물체에 반사되어 돌아온 초음파의 시간을 저장   

  cm = microsecondsToCm(duration);
  distance = cm;

  client.loop();
  
   if(SNIPE.lora_recv()=="test1"){
    DebugSerial.println("Receive Data and Bus depart as soon");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("02BUS will");
      lcd.setCursor(0,1);
      lcd.print("arrive soon!!");
      
      delay(3000);
      lcd.clear();                     
   }
  delay(2000);

  if(cm < 30){
    count = ++SR_count;
    DebugSerial.println("Event Occuracy!");
    DebugSerial.print(cm); 
    DebugSerial.print(" cm, SR count : ");
    DebugSerial.print(SR_count);
    DebugSerial.println();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Current Request");
    lcd.setCursor(0,1);
    lcd.print("       : ");
    lcd.print(SR_count);

    if (millis() - lastMillis > 5000){
    lastMillis = millis();
    publishMessage();
   }     
    if(SR_count == 11){
      delay(2000);
      SR_count = 0;
    }
} 
   else{
     count = SR_count;
     lcd.clear();
     DebugSerial.println("No Event Occuracy!");
     DebugSerial.print(cm); 
     DebugSerial.println(" cm");
     DebugSerial.println("\n");
     
    if (millis() - lastMillis > 5000){
    lastMillis = millis();
    publishMessage();
   }     
}
}

void connectAWS(){
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  DebugSerial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
 
  NTPConnect();
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);

  DebugSerial.println("Connecting to AWS IOT");
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  } 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  
  DebugSerial.println("AWS IoT Connected!");
}

void messageReceived(char *topic, byte *payload, unsigned int length) {
  DebugSerial.print("Received [");
  DebugSerial.print(topic);
  DebugSerial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    DebugSerial.print((char)payload[i]);
  }
  DebugSerial.println();

      if(SNIPE.lora_send("test")){
      DebugSerial.print("\n\nClear To Send!!");
      DebugSerial.println("\n");
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Send Signal");
      lcd.setCursor(0,1);
      lcd.print("to Busgarage!");

      delay(3000);
      }      
      else {
      DebugSerial.print("\n\nTransmit error");
      DebugSerial.println("\n");
      
      lcd.setCursor(0,0);
      lcd.print("Transmit");
      lcd.setCursor(0,1);
      lcd.print("Error");  
      
      delay(3000);
      }
}

void publishMessage(void){
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["distance"] = ::distance;
  doc["SR_count"] = count;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

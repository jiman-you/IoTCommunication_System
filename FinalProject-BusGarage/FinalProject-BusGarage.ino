#include <SoftwareSerial.h>
#include "SNIPE.h"

#define PING 1
#define PONG 2 

#define CODE PONG

#define TXpin 11
#define RXpin 10
#define ATSerial Serial

int piezoPin =6;
int button =8;


//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  

SoftwareSerial DebugSerial(RXpin, TXpin);
SNIPE SNIPE(ATSerial);

void setup(){
  pinMode(piezoPin, OUTPUT);
  pinMode(button, INPUT);
  ATSerial.begin(115200); // 로라 Serial 속도 설정 
  DebugSerial.begin(115200);
  
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
  DebugSerial.println("Buzzer TEST START!");
}

void loop(){
  DebugSerial.println("Waiting to SR Data..\n");

  if(SNIPE.lora_recv()=="test"){
    DebugSerial.print("Receive Success ");
    DebugSerial.println();
    
    for(int i = 0; i < 5; i++){
        tone(piezoPin,700);
        delay(1000);
        tone(piezoPin,850);
        delay(1000);
      }

  }

  if( digitalRead(button)==HIGH){
    if(SNIPE.lora_send("test1")){
      DebugSerial.println("\t\t\tButton Pressed and Bus depart! with sending LCD DATA!");
      DebugSerial.println();
    }
    else{
      DebugSerial.println("\t\t\tButton Pressed and Bus depart! BUT, sending error");
      DebugSerial.println();
    }
  }
  else{
    DebugSerial.println("Receive Fail\n");
    noTone(piezoPin);
  }
  
  delay(1000);
  }
  

#include <SoftwareSerial.h>
#include <avr/sleep.h>                  // スリープライブラリ
#include <avr/wdt.h>                    // ウォッチドッグタイマー ライブラリ

#define LORA_RECV_RecvData 100

#define RST_PIN 10
#define SLEEP_PIN 11

#define DEPTH 3

bool my_node = false;

//D:0.067448889
//E:0.072391112
//F:0.06647111

const float cycle = 0.06647111;

unsigned int sleepTime = 3600;
//unsigned int Time = 1770;
//unsigned int sleepTime = 3600;
const int SetCmdDelay = 1000; /*待機時間*/

const int MaxSendTimes = 1; /*最大送信回数*/
static const int LoRa_Rx = 2, LoRa_Tx = 3;
bool received = false;
bool flag = true;
SoftwareSerial LoRaSerial(LoRa_Rx, LoRa_Tx);

void setup() {
  Serial.begin(9600);
  LoRaSerial.begin(9600);
  pinMode(SLEEP_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, HIGH);
  resetFunc();

  delay(3000);

  clearBuffer();
  delay(200);
  LoRaSerial.println('2');
  delay(200);
  LoRaSerial.println("d 2\r\n");
  delay(200);
  LoRaSerial.println('z');
  delay(200);

//  Serial.println("0001FFFFaaaa");
  delay(3000);
  LoRaSerial.println("0001FFFFsleep");
//  delay(3000);
  Serial.println("Set up OK");
  clearBuffer();
}

void loop() {
  unsigned int i;
  unsigned int cnt = 0;
  char RecvData[LORA_RECV_RecvData] = "";
  char EditedData[LORA_RECV_RecvData] = "";
  char recvdata[LORA_RECV_RecvData] = "";
  char editeddata[LORA_RECV_RecvData] = "";

  char SendData = ""; /*送信内容*/
  char nextSendAddr[9] = "0001FFFF";
  char Rssi[4] = "";
  unsigned char n = 0;
  signed int Rssi10 = 0;
  unsigned int chennel[DEPTH] = {1, 2, 3};

  
  digitalWrite(SLEEP_PIN, LOW); // LoRaの起床
  switchingChannel(chennel[1]);
  digitalWrite(SLEEP_PIN, HIGH); // LoRaの就寝

  if (flag) {
    for (i = 0; i < sleepTime / cycle; i++) {
      delayWDT2(2);                         // 引数でスリープ時間指定（詳細はdelayWDT_setup参照）
    }
  } else {
    for (i = 0; i < (sleepTime - 45) / cycle; i++) {
      delayWDT2(2);                         // 引数でスリープ時間指定（詳細はdelayWDT_setup参照）
    }
  }

//  for (i = 0; i < sleepTime / cycle; i++) {
//    delayWDT2(2);                         // 引数でスリープ時間指定（詳細はdelayWDT_setup参照）
//  }
  digitalWrite(SLEEP_PIN, LOW); // LoRaの起床
//  delay(5000);

//  LoRaSerial.println("0001FFFFhello");
//  delay(10000);

  clearBuffer();

  while (true) {
    if (++cnt > 34 && flag) { //20:40s 1s=1.5
      delay(1000);
      flag = false;
      break;
    }
    
    if (++cnt > 44 && !flag) { //30:55s 34:60.5s
      delay(1000);
      flag = true;
      break;
    }
    // 受信プログラム
    while (LoRaSerial.available() > 0) {
      getReciveData(recvdata);
    }
    delay(1000);
    if (received) {
//      LoRaSerial.println("0001FFFFuketotta");
      delay(300);
      strCut(recvdata, editeddata); //上位８文字を除去
//      my_node = nodeDecision(editeddata);
      Serial.println(editeddata);
      switchingChannel(chennel[2]);
//      if (!my_node) {
      sendMessage(editeddata, nextSendAddr);
//      }
//      else {
//        Serial.println("own_node");
//        clearBuffer();
//      }
      received = false;
      flag = true;
      cnt = 0;
      break;  
    }
    delay(2000);
  }    
}

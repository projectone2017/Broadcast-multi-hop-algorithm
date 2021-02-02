#include <SoftwareSerial.h>
#include <avr/sleep.h>                  // スリープライブラリ
#include <avr/wdt.h>                    // ウォッチドッグタイマー ライブラリ

#define LORA_RECV_RecvData 100

#define RST_PIN 10
#define SLEEP_PIN 11

#define DEPTH 3

bool my_node = false;

//A:0.067004443
//B:0.068355556
//C:0.067822221
//D:0.067448889
//E:0.072391112
//F:0.06647111

const float cycle = 0.068355556;

//unsigned int sleepTime = 1800;
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
  LoRaSerial.println("d 1\r\n");
  delay(200);
  LoRaSerial.println('z');
  delay(200);

  //  Serial.println("0001FFFFaaaa");
  LoRaSerial.println("0001FFFFsleep");
  delay(3000);
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
  char nextSendAddr[12] = "0001FFFFfs6";
  char Rssi[4] = "";
  unsigned char n = 0;
  signed int Rssi10 = 0;
  unsigned int chennel[DEPTH] = {1, 2, 3};

  // 受信プログラム
  while (LoRaSerial.available() > 0) {
    getReciveData(recvdata);
  }
  delay(300);
  if (received) {
    delay(300);
    strCut(recvdata, editeddata); //上位８文字を除去
    Serial.println(editeddata);
    switchingChannel(chennel[1]);
    sendMessage(editeddata, nextSendAddr);

    received = false;
    flag = true;
    cnt = 0;

    //    delay(2000);
    digitalWrite(SLEEP_PIN, LOW); // LoRaの起床
    switchingChannel(chennel[0]);
    digitalWrite(SLEEP_PIN, HIGH); // LoRaの就寝

    delay(3000);
    digitalWrite(SLEEP_PIN, LOW); // LoRaの起床
  }
}

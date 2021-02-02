void clearBuffer() {
  delay(SetCmdDelay);
  while (LoRaSerial.available() > 0) LoRaSerial.read();
}

void resetFunc(void) {
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, LOW);
  delay(1000);
  digitalWrite(RST_PIN, HIGH);
  delay(1000);
}

void LoRaInit() {
  //delay(SetCmdDelay);
  LoRaSerial.begin(9600);
  delay(SetCmdDelay);
  Serial.print("Start...");
  // コマンドモード開始
  LoRaSerial.write("2\r\n"); clearBuffer();
  Serial.print("2");
  // bw（帯域幅の設定）
  LoRaSerial.write("bw 4\r\n"); clearBuffer();
  // sf（拡散率の設定）
  Serial.print("bw");
  LoRaSerial.write("sf 12\r\n"); clearBuffer();
  //  チャンネルの設定　smartcityは5,6,7
  Serial.print("sf");
  LoRaSerial.write("d 1\r\n"); clearBuffer();
  // 自分が参加するPANネットワークアドレスの設定
  
  LoRaSerial.write("c 0001\r\n"); clearBuffer();
  // 自分のノードIDを設定
  
  LoRaSerial.write("ownid 000");
  
  LoRaSerial.println(node); clearBuffer();
  // 相手のノードIDを設定
  LoRaSerial.write("g 0001\r\n"); clearBuffer();
  //送信元のID付与設定
  LoRaSerial.write("o 1\r\n"); clearBuffer();
  // RRSIの付与設定
  LoRaSerial.write("p 2\r\n"); clearBuffer();
  // 送信モードを設定
  LoRaSerial.write("n 2\r\n"); clearBuffer();
  //Ack受信を行うか
  LoRaSerial.write("l 2\r\n"); clearBuffer();
  // 設定を保存する
  LoRaSerial.write("w\r\n"); clearBuffer();
  // 通信の開始
  LoRaSerial.write("z\r\n"); clearBuffer();
  delay(3000);
  Serial.println("Set up OK!");
  clearBuffer();
}

char *strCut(char *str, char *buf) {
  int i;
  for (i = 8; i < strlen(str); i++) {
    buf[i - 8] = str[i];
  }
  return buf;
}

bool nodeDecision(char *recv_data) {
  char is_donw = recv_data[0];
  int dst_id = atoi(&recv_data[1]);
  int i;
  if (node == dst_id) {
    return true;
  }
  else {
    return false;
  }

}

void sendMessage(char *msg, char *addr) {
  char sendData[58];
  clearBuffer(); // 念のためバッファクリア
  sprintf(sendData, "%s%s", addr, msg);
  delay(1000);
//  Serial.println(sendData);
  LoRaSerial.println(sendData);
  delay(500);
  LoRaSerial.println(sendData);
  delay(500);
  LoRaSerial.println(sendData);
  delay(500);
  LoRaSerial.println(sendData);
  delay(500);
  LoRaSerial.println(sendData);
  clearBuffer();
}

void *getReciveData(char *RecvData) {
  unsigned char n = 0;
  while (LoRaSerial.available() > 0) {
    RecvData[n] = LoRaSerial.read();
    if (++n > LORA_RECV_RecvData) n = 0; //スタックオーバーフロー対策
    if (RecvData[n - 1] == '\r' || RecvData[n - 1] == '\n') {
      //      RecvData[n - 1] = "\0";
      if (RecvData[n - 1] == 0 || RecvData[n - 1] == "0") return;
      //受信データの表示
      received = true;
      return;
    }
  }
}


void switchingChannel(int channel) {
  resetFunc();
  clearBuffer();
  LoRaSerial.write("2\r\n"); clearBuffer();
  LoRaSerial.write("d "); clearBuffer();
  LoRaSerial.println(channel); clearBuffer();
  LoRaSerial.write("w\r\n"); clearBuffer();
  LoRaSerial.write("z\r\n"); clearBuffer();
  delay(500);
}

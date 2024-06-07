#include <M5Unified.h>
// SDカード関連関連
#include <SD.h>
// グラフィック関連
#include <M5GFX.h>
#include <Avatar.h>
#include "select-face/AllFaces.h"
#include "select-face/LaputaFace.h"
// サーボモーター関連
#include <ServoEasing.hpp> // https://github.com/ArminJo/ServoEasing       
#include <Ticker.h>
// QRコードリーダー関連
#include <M5UnitQRCode.h>
// Wifi関連
#include <WiFiClientSecure.h>
// オーディオ関連
#include "Audio.h"
#include "AudioOutputM5Speaker.h"
#include "AudioGeneratorMP3.h"
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include <ArduinoJson.h>
#include "AudioFileSourceHTTPSStream.h"
// Voicevox関連
#include "WebVoiceVoxTTS.h"

using namespace m5avatar;

#if defined( ARDUINO_M5STACK_Core2 )
  // M5Stack Core2
  // バーコードリーダー　Port.C X:G13, Y:G14
  // サーボモーター　　　Port.A X:G33, Y:G32
  // スタックチャン基板 X:G19, Y:G27
  #define QR_PIN_1 14
  #define QR_PIN_2 13
  #define SERVO_PIN_X 33
  #define SERVO_PIN_Y 32
#elif defined( ARDUINO_M5STACK_FIRE )
  // M5Stack Fire
  // バーコードリーダー　Port.C(X:G17, Y:G16)
  // サーボモーター　　　Port.A(X:G22, Y:G21)
  // I2Cと同時利用は不可
  #define QR_PIN_1 22
  #define QR_PIN_2 21
  #define SERVO_PIN_X 22
  #define SERVO_PIN_Y 21
#if SERVO_PIN_X == 22
  // FireでPort.Aを使う場合は内部I2CをOffにする必要がある。
  #define CORE_PORT_A
#endif
#endif

Avatar avatar;
ColorPalette *cp;

// Wifi & VOICEVOX_APIKEY
// const char *SSID = "aaa";
// const char *PASSWORD = "aaa";
// #define VOICEVOX_APIKEY "aaa"

// Voicevox用パラメータ
String VOICEVOX_API_KEY = "";
String TTS_SPEAKER_NO = "58"; // 58 : 猫使ビィ　ノーマル
String TTS_SPEAKER = "&speaker=";
String TTS_PARMS = TTS_SPEAKER + TTS_SPEAKER_NO;
String speech_text = "";

// バーコードリーダー用変数
M5UnitQRCodeI2C qrcode_i2c;
// M5UnitQRCodeUART qrcode_uart; // UARTは不使用

// バーコード関連用変数
uint8_t buffer[512] = {0};
uint16_t length     = 0;
unsigned long long barcode = 0;

// 顔パーツの種類（いったん目、眉、口は全部同じ数だけある想定
int parts_num = 10;
int tone_def = 1000;
int tone_dif = 100;

// 発話テキスト選択
String select_speech(int slct_num) {
  static String response = "";
  if (slct_num == 0) {
    // 0: ノーマル : 3 : ずんだもん（ノーマル）
    response = "どうもどうも、　僕は　スタックチャンです。";
  }
  else if (slct_num == 1) {
    // 1: にょろん : 54 : WhiteCUL（たのしい）
    response = "やめられない、　とまらない、　スタックチャンでーーす";
  }
  else if (slct_num == 2) {
    // 2: ダンポー : 42 : ちび式じい
    response = "あきらめたら　そこで試合終了ですよ･･･？";
  }
  else if (slct_num == 3) {
    // 3: くちびる : 66 :  : もち子さん（セクシー）
    response = "春はあけぼの。やうやう白く、なりゆくうっふん";
  }
  else if (slct_num == 4) {
    // 4: うわーん : 76 :  : ずんだもん（なみだめ）
    response = "ウニャーーーーーー";
  }
  else if (slct_num == 5) {
    // 5: ちいかわ : 62 : 中国うさぎ（おどろき）
    response = "どうしよう、このお店、現金しか使えないや！！";
  }
  else if (slct_num == 6) {
    // 6: めがね　 : 0 : 四国めたん
    response = "ムシャムシャしてやった、　今ははんすうしている。";
  }
  else if (slct_num == 7) {
    // 7: けんしろ : 83 :  : 青山劉生（喜び）
    response = "お前はもう　しんでいる！";
  }
  else if (slct_num == 8) {
    // 8: ケイ     : 49 : ナースロボ（恐怖）
    response = "ピーー　ガガガガガ　ピーーーーー！　　　　　　　なんちゃって";
  }
  else if (slct_num == 9) {
    // 9: アリス   : 48 : ナースロボ（楽楽）
    response = "ブラジルのみなさーーーん　きこえますかーーーー";
  }
  else {
    // スペシャル顔＝ラピュタロボは発話しない
    response = "";
  }
  Serial.println("====================");
  Serial.println(response);
  Serial.println("====================");
  return response;
}

// 声帯選択
String select_voice(int slct_num) {
  static String response = "";
  if (slct_num == 0) {
    // 0: ノーマル : 3 : ずんだもん（ノーマル）
    response = "3";
  }
  else if (slct_num == 1) {
    // 1: にょろん : 24 : WhiteCUL（たのしい）
    response = "54";
  }
  else if (slct_num == 2) {
    // 2: ダンポー : 42 : ちび式じい
    response = "42";
  }
  else if (slct_num == 3) {
    // 3: くちびる : 66 :  : もち子さん（セクシー）
    response = "66";
  }
  else if (slct_num == 4) {
    // 4: うわーん : 76 :  : ずんだもん（なみだめ）
    response = "76";
  }
  else if (slct_num == 5) {
    // 5: ちいかわ : 62 : 中国うさぎ（おどろき）
    response = "62";
  }
  else if (slct_num == 6) {
    // 6: めがね　 : 0 : 四国めたん
    response = "0";
  }
  else if (slct_num == 7) {
    // 7: けんしろ : 83 :  : 青山劉生（喜び）
    response = "83";
  }
  else if (slct_num == 8) {
    // 8: ケイ     : 49 : ナースロボ（恐怖）
    response = "49";
  }
  else if (slct_num == 9) {
    // 9: アリス   : 48 : ナースロボ（楽楽）
    response = "48";
  }
  else {
    // スペシャル顔＝ラピュタロボは発話しない
    response = "";
  }
  Serial.println("====================");
  Serial.println(response);
  Serial.println("====================");
  return response;
}

// アバター関連初期化
void init_avatar()
{
  avatar.setFace(new AllFaces());
  avatar.setEyeType(0);
  avatar.setEyeBlowType(0);
  avatar.setMouthType(0);
  TTS_SPEAKER_NO = select_voice(0);
  speech_text = select_speech(0);
  cp->set(COLOR_PRIMARY, TFT_BLACK);
  cp->set(COLOR_BACKGROUND, TFT_WHITE);
  avatar.setColorPalette(*cp);
}

// 指定アバター選択
void select_avatar(int num)
{
  avatar.setFace(new AllFaces());
  avatar.setEyeType(num);
  avatar.setEyeBlowType(num);
  avatar.setMouthType(num);
  TTS_SPEAKER_NO = select_voice(num);
  speech_text = select_speech(num);
}

// バーコード関連初期化
void init_barcode()
{
  buffer[512] = {0};
  length      = 0;
  barcode     = 0;
}

// 顔選択用ID
int slct = 0;

// サーボモーター用定義
#define START_DEGREE_VALUE_X 90
#define START_DEGREE_VALUE_Y 90
ServoEasing servo_x;
ServoEasing servo_y;
int servo_offset_x = 0;  // X軸サーボのオフセット（90°からの+-で設定）
int servo_offset_y = 0;  // Y軸サーボのオフセット（90°からの+-で設定）

// サーボモーター：XY位置指定
void moveXY(int x, int y, uint32_t millis_for_move = 0) {
  if (millis_for_move == 0) {
    servo_x.setEaseTo(x + servo_offset_x);
    servo_y.setEaseTo(y + servo_offset_y);
  } else {
    servo_x.setEaseToD(x + servo_offset_x, millis_for_move);
    servo_y.setEaseToD(y + servo_offset_y, millis_for_move);
  }
  // サーボが停止するまでウェイトします。
  synchronizeAllServosStartAndWaitForAllServosToStop();
}

// サーボモーター：ランダム挙動
void moveRandom() {
  int cnt = 0;
  for (;;) {
    // ランダムモード
    int x = random(55, 125);  // 45〜135° でランダム
    int y = random(50, 75);   // 50〜90° でランダム
#ifdef ARDUINO_M5STACK_CORES3
    unifiedButton.update(); // M5.update() よりも前に呼ぶ事
#endif
/*
    M5.update();
    if (M5.BtnC.wasPressed()) {
      break;
    }
*/
    int delay_time = random(10);
    moveXY(x, y, 1000 + 100 * delay_time);
    delay(2000 + 500 * delay_time);
    avatar.setBatteryStatus(M5.Power.isCharging(), M5.Power.getBatteryLevel());
    //avatar.setSpeechText("Stop BtnC");
    avatar.setSpeechText("");

    cnt++;
    if (cnt == 3) {
      Serial.println("Stop move");
      moveXY(START_DEGREE_VALUE_X, START_DEGREE_VALUE_Y, 500);
      break;
    }
  }
}

//---------------------------------------------

/// set M5Speaker virtual channel (0-7)
static constexpr uint8_t m5spk_virtual_channel = 0;
static AudioOutputM5Speaker out(&M5.Speaker, m5spk_virtual_channel);
AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;

// 発話実行（mp3再生）
void playMP3(AudioFileSourceBuffer *buff){
  mp3->begin(buff, &out);
}

// 音量に合わせた口の開閉
void lipSync(void *args)
{
  float gazeX, gazeY;
  int level = 0;
  DriveContext *ctx = (DriveContext *)args;
  Avatar *avatar = ctx->getAvatar();
  for (;;)
  {
    level = abs(*out.getBuffer());
    if(level<100) level = 0;
    if(level > 15000)
    {
      level = 15000;
    }
    float open = (float)level/15000.0;
    float fix_o = -1*open*open+2*open; // 口の開閉を線形から非線形に微調整
#ifdef DEBUG
    // デバッグ用：口の開き具合 0.00 - 1.00
    Serial.println(fix_o);
#endif // DEBUG
    avatar->setMouthOpenRatio(fix_o);
    avatar->getGaze(&gazeY, &gazeX);
    avatar->setRotation(gazeX * 5);
    delay(50);
  }
}


void setup() {

  // M5 Config初期設定
  auto cfg = M5.config();
  cfg.external_spk = true;    /// use external speaker (SPK HAT / ATOMIC SPK)
  M5.begin(cfg);
  { /// custom setting
    auto spk_cfg = M5.Speaker.config();
    /// Increasing the sample_rate will improve the sound quality instead of increasing the CPU load.
    spk_cfg.sample_rate = 96000; // default:64000 (64kHz)  e.g. 48000 , 50000 , 80000 , 96000 , 100000 , 128000 , 144000 , 192000 , 200000
    spk_cfg.task_pinned_core = APP_CPU_NUM;
    M5.Speaker.config(spk_cfg);
  }
  M5.Speaker.begin();
  M5.Speaker.setVolume(200);
  mp3 = new AudioGeneratorMP3();

  M5.update();

  // SDカード/SPIからデータ読み取り
  //  WiFi.begin(SSID, PASSWORD);
  if (SD.begin(GPIO_NUM_4, SPI, 25000000)) {
    // Wifi 初期設定
    WiFi.disconnect();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    Serial.println("Connecting to WiFi");
    // SDカードからWifi接続用の情報読み取り
    auto fs = SD.open("/wifi.txt", FILE_READ);
    if(fs) {
      size_t sz = fs.size();
      char buf[sz + 1];
      fs.read((uint8_t*)buf, sz);
      buf[sz] = 0;
      fs.close();

      int y = 0;
      for(int x = 0; x < sz; x++) {
        if(buf[x] == 0x0a || buf[x] == 0x0d)
          buf[x] = 0;
        else if (!y && x > 0 && !buf[x - 1] && buf[x])
          y = x;
      }
      WiFi.begin(buf, &buf[y]);
    } else {
       WiFi.begin();
    }
    // 接続処理中
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println("...Connecting to WiFi");
      delay(1000);   
    }
    Serial.println("Connected");

    // APIキー 初期設定
    fs = SD.open("/apikey.txt", FILE_READ);
    if(fs) {
      size_t sz = fs.size();
      char buf[sz + 1];
      fs.read((uint8_t*)buf, sz);
      buf[sz] = 0;
      fs.close();

      int y = 0;
      int z = 0;
      for(int x = 0; x < sz; x++) {
        if(buf[x] == 0x0a || buf[x] == 0x0d)
          buf[x] = 0;
        else if (!y && x > 0 && !buf[x - 1] && buf[x])
          y = x;
        else if (!z && x > 0 && !buf[x - 1] && buf[x])
          z = x;
      }
      //nvs_set_str(nvs_handle, "openai", buf);
      //nvs_set_str(nvs_handle, "voicevox", &buf[y]);
      //nvs_set_str(nvs_handle, "sttapikey", &buf[z]);
      Serial.println("------------------------");
      Serial.println(buf); // ChatGPT
      Serial.println(&buf[y]); // VOICEVOX
      Serial.println(&buf[z]); // STT
      Serial.println("------------------------");
      // VOICEVOXのAPIキー設定
      VOICEVOX_API_KEY = &buf[y];
    }
  }

  // M5Stack Avatar初期設定
  cp = new ColorPalette();
  cp->set(COLOR_PRIMARY, TFT_BLACK);
  cp->set(COLOR_BACKGROUND, TFT_WHITE);
  avatar.setColorPalette(*cp);
  avatar.setFace(new AllFaces());
  avatar.init(8);
  avatar.addTask(lipSync, "lipSync");
  init_avatar();

  // M5UnitQRCode初期設定
  while (!qrcode_i2c.begin(&Wire, UNIT_QRCODE_ADDR, QR_PIN_1, QR_PIN_2, 100000U)) {
      Serial.println("Unit QRCode I2C Init Fail");
      delay(1000);
  }
  Serial.println("Unit QRCode I2C Init Success");
  Serial.println("Manual Scan Mode");
  qrcode_i2c.setTriggerMode(MANUAL_SCAN_MODE);

  // サーボモーター初期設定
  if (servo_x.attach(SERVO_PIN_X, 
                     START_DEGREE_VALUE_X + servo_offset_x,
                     DEFAULT_MICROSECONDS_FOR_0_DEGREE,
                     DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo x\n");
  }
  Serial.print("Success attached servo x\n");
  if (servo_y.attach(SERVO_PIN_Y,
                     START_DEGREE_VALUE_Y + servo_offset_y,
                     DEFAULT_MICROSECONDS_FOR_0_DEGREE,
                     DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo y\n");
  }
  Serial.print("Success attached servo y\n");
  servo_x.setEasingType(EASE_QUADRATIC_IN_OUT);
  servo_y.setEasingType(EASE_QUADRATIC_IN_OUT);
  setSpeedForAllServos(60);

  // サーボモーター初期位置設定
  moveXY(START_DEGREE_VALUE_X, START_DEGREE_VALUE_Y, 500);
  Serial.println("Init posisioning");

}

void loop() {

  // バーコードを読み取った場合に値を変数に格納
  if (qrcode_i2c.getDecodeReadyStatus() == 1) {
    length = qrcode_i2c.getDecodeLength();
    Serial.printf("len:%d\r\n", length);
    qrcode_i2c.getDecodeData(buffer, length);
    Serial.printf("decode data:");
    for (int i = 0; i < length; i++) {
        Serial.printf("%c", buffer[i]);
    }
    Serial.println();
    for (int i = 0; i < length; i++) {
        barcode = buffer[i] - '0' + barcode * 10;
    }
  }

  // 取得したバーコードの値を吹き出しに表示
  if (barcode > 0) {
    char buff[100];
    sprintf(buff, "%lld", barcode);
    avatar.setSpeechText(buff);
  }

  M5.update();
  /**
   * バーコード読み取りモード～顔ランダムセレクト状態～顔決定
  */
  if (M5.BtnA.wasPressed())
  {
    M5.Speaker.tone(1000, 100);
    avatar.setSpeechText("");

#ifdef DEBUG
    // デバッグ用：スペシャル顔
    barcode = 12345678910133;
#endif // DEBUG

    if (barcode > 0) {
      // アバター関連初期化
      init_avatar();

      // バーコードで読み取った値(EAN型 13桁)からパーツIDを決定
      int mod1 = barcode % 9973; // 10000に近い素数で割った余り
      int mod2_e = mod1 % 13;
      int mod2_eb = mod1 % 17;
      int mod2_m = mod1 % 19;
      // スペシャル顔決定フラグの設定
      int mod2_sp = mod1 % 23;

#ifdef DEBUG
      // デバッグ用：スペシャル顔フラグの値確認コード
      char buff[100];
      sprintf(buff, "ID : %d", mod2_sp);
      avatar.setSpeechText(buff);
#endif // DEBUG

      int your_e = 0;
      int your_eb = 0;
      int your_m = 0;
      bool sp_face = false;
      if (mod2_sp == 21) {
        // もし余りが21だったらスペシャル顔をセット
        sp_face = true;
      } else {
        // パーツID決定
        your_e = mod2_e % parts_num;
        your_eb = mod2_eb % parts_num;
        your_m = mod2_m % parts_num;
      }

      // 顔ランダムセレクト状態
      bool face_drum_rotation = true;
      int rnd_e = 0;
      int rnd_eb = 0;
      int rnd_m = 0;
      int drum_cnt = 0;
      int tone_vol = tone_def;
      while (face_drum_rotation) {

        M5.Speaker.tone(tone_vol, 100);

        rnd_e = rand() % parts_num;
        rnd_eb = rand() % parts_num;
        rnd_m = rand() % parts_num;
        avatar.setEyeType(rnd_e);
        avatar.setEyeBlowType(rnd_eb);
        avatar.setMouthType(rnd_m);
        delay(300);
        drum_cnt++;
        tone_vol = tone_vol + tone_dif;
        // 顔パーツをセット
        if (drum_cnt == 12) {
          face_drum_rotation = false;
          if (sp_face) {
            // スペシャル顔をセット
            cp->set(COLOR_PRIMARY, TFT_WHITE);
            cp->set(COLOR_BACKGROUND, TFT_BLACK);
            avatar.setColorPalette(*cp);
            avatar.setFace(new LaputaFace());
            // 声をセット
          } else {
            // 個別の顔パーツをセット
            avatar.setEyeType(your_e);
            avatar.setEyeBlowType(your_eb);
            avatar.setMouthType(your_m);
            // 口の形に合わせて声とテキストをセット
            TTS_SPEAKER_NO = select_voice(your_m);
            speech_text = select_speech(your_m);
          }
          M5.Speaker.tone(tone_vol, 500);
          break;
        }
      }
      // 一言しゃべる
      Serial.println("Speech");
      if (!mp3->isRunning()) {
        if(speech_text != "") {
          TTS_PARMS = TTS_SPEAKER + TTS_SPEAKER_NO;
          Voicevox_tts((char*)speech_text.c_str(), (char*)TTS_PARMS.c_str());
          speech_text = "";
        } else {
          // スペシャル顔はしゃべらない
          delay(500);
          M5.Speaker.tone(500, 100);
          delay(300);
          M5.Speaker.tone(2000, 100);
          delay(600);
          M5.Speaker.tone(1500, 100);
          delay(300);
          M5.Speaker.tone(1250, 100);
          delay(600);
          M5.Speaker.tone(500, 100);
          delay(300);
          M5.Speaker.tone(2000, 100);
        }
      }
      init_barcode();
    } else {
      avatar.setSpeechText("Read Barcode Plz!");
    }
  }

  /**
   * 顔パターン表示テスト
  */
  if (M5.BtnB.wasPressed())
  {
    M5.Speaker.tone(1500, 100);
    avatar.setSpeechText("");

    // アバター関連初期化
    init_avatar();
    // バーコード関連初期化
    init_barcode();
    slct = slct + 1;
    if (slct == 10) {
      cp->set(COLOR_PRIMARY, TFT_WHITE);
      cp->set(COLOR_BACKGROUND, TFT_BLACK);
      avatar.setColorPalette(*cp);
      avatar.setFace(new LaputaFace());
      speech_text = select_speech(slct);
    } else if(slct == 11) {
      slct = 0;
      avatar.setFace(new AllFaces());
      select_avatar(slct);
      TTS_SPEAKER_NO = select_voice(slct);
      speech_text = select_speech(slct);
    } else {
      select_avatar(slct);
      TTS_SPEAKER_NO = select_voice(slct);
      speech_text = select_speech(slct);
    }
  }

  /**
   * ランダム動作 ON/OFF
  */
  if (M5.BtnC.pressedFor(1000)) {
    M5.Speaker.tone(1000, 100);
    Serial.println(" Btn Pressed ");
    delay(200);
    M5.Speaker.end();

    Serial.println("Speech");
    if (!mp3->isRunning()) {
      if(speech_text != "") {
        TTS_PARMS = TTS_SPEAKER + TTS_SPEAKER_NO;
        Voicevox_tts((char*)speech_text.c_str(), (char*)TTS_PARMS.c_str());
        speech_text = "";
      } else {
        // スペシャル顔はしゃべらない
        delay(500);
        M5.Speaker.tone(500, 100);
        delay(300);
        M5.Speaker.tone(2000, 100);
        delay(600);
        M5.Speaker.tone(1500, 100);
        delay(300);
        M5.Speaker.tone(1250, 100);
        delay(600);
        M5.Speaker.tone(500, 100);
        delay(300);
        M5.Speaker.tone(2000, 100);
      }
    }
  }

  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
      if(file != nullptr){delete file; file = nullptr;}
      Serial.println("mp3 stop");
      Serial.println("Randum move");
      moveRandom();
    }
    delay(1);
  }
}

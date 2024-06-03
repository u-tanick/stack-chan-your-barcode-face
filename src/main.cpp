#include <M5Unified.h>
#include <M5GFX.h>
#include <Avatar.h>
#include <Ticker.h>
#include <ServoEasing.hpp> // https://github.com/ArminJo/ServoEasing       
#include <M5UnitQRCode.h>

#include "select-face/AllFaces.h"
#include "select-face/LaputaFace.h"

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
M5UnitQRCodeI2C qrcode_i2c;
M5UnitQRCodeUART qrcode_uart;

// バーコード関連用変数
uint8_t buffer[512] = {0};
uint16_t length     = 0;
unsigned long long barcode = 0;

// 顔パーツの種類（いったん目、眉、口は全部同じ数だけある想定
int parts_num = 10;
int tone_def = 1000;
int tone_dif = 100;

// アバター関連初期化
ColorPalette *cp;
void init_avatar()
{
  avatar.setFace(new AllFaces());
  avatar.setEyeType(0);
  avatar.setEyeBlowType(0);
  avatar.setMouthType(0);
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
    int x = random(45, 135);  // 45〜135° でランダム
    int y = random(60, 90);   // 50〜90° でランダム
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
    if (cnt == 5) {
      Serial.println("Stop move");
      moveXY(START_DEGREE_VALUE_X, START_DEGREE_VALUE_Y, 500);
      break;
    }
  }
}

void setup() {
  M5.begin();

  // M5Stack Avatar初期設定
  cp = new ColorPalette();
  cp->set(COLOR_PRIMARY, TFT_BLACK);
  cp->set(COLOR_BACKGROUND, TFT_WHITE);
  avatar.setColorPalette(*cp);
  avatar.setFace(new AllFaces());
  avatar.init(8);

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
    Serial.print("Error attaching servo x");
  }
  if (servo_y.attach(SERVO_PIN_Y,
                     START_DEGREE_VALUE_Y + servo_offset_y,
                     DEFAULT_MICROSECONDS_FOR_0_DEGREE,
                     DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo y");
  }
  servo_x.setEasingType(EASE_QUADRATIC_IN_OUT);
  servo_y.setEasingType(EASE_QUADRATIC_IN_OUT);
  setSpeedForAllServos(60);

  // サーボモーター初期位置設定
  moveXY(START_DEGREE_VALUE_X, START_DEGREE_VALUE_Y, 500);
  Serial.println("Randum move");

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
        if (drum_cnt == 12) {
          // 顔パーツをセット
          face_drum_rotation = false;
          if (sp_face) {
            // スペシャル顔をセット
            cp->set(COLOR_PRIMARY, TFT_WHITE);
            cp->set(COLOR_BACKGROUND, TFT_BLACK);
            avatar.setColorPalette(*cp);
            avatar.setFace(new LaputaFace());
          } else {
            // 個別の顔パーツをセット
            avatar.setEyeType(your_e);
            avatar.setEyeBlowType(your_eb);
            avatar.setMouthType(your_m);
          }
          M5.Speaker.tone(tone_vol, 500);
          break;
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
//    avatar.setFace(new AllFaces());
    slct = slct + 1;
    if (slct == 10) {
      cp->set(COLOR_PRIMARY, TFT_WHITE);
      cp->set(COLOR_BACKGROUND, TFT_BLACK);
      avatar.setColorPalette(*cp);
      avatar.setFace(new LaputaFace());
    } else if(slct == 11) {
      slct = 0;
      select_avatar(slct);
    } else {
      select_avatar(slct);
    }
  }

  /**
   * ランダム動作 ON/OFF
  */
  if (M5.BtnC.pressedFor(1000)) {
    M5.Speaker.tone(2000, 100);
    avatar.setSpeechText("");
    Serial.println("Randum move");
    moveRandom();
  }
}

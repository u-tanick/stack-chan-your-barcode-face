# stack-chan-your-barcode-face

M5Stack の QRコードスキャナーユニット（STM32F030）を使用してバーコードからスタックチャンの顔をランダムで作成します。

顔は　目　眉　口　それぞれ　10種類あり、バーコードのナンバーをもとにそれぞれランダムで決定されます。

## platformio.iniの修正

Cloneした環境に合わせて、25行目のフォルダパスを正しく設定してください。今回用に改造したm5stack-avatarを使っているため、個別にライブラリのパスを通す必要があります。

> <終端フォルダまでのパス>C:\Users\hoge\Documents\PlatformIO\Projects\stack-chan-your-barcode-face\lib\m5stack-avatar-custom-u-tanick

## STM32F030の設定方法

M5Stack Core2 AWS版に、以下のように接続します。

- Aポート：サーボモーター
- Cポート：STM32F030

バーコードは、　EAN-13　を想定しており、以下のサイトから任意のバーコードを作成することが可能です。

https://u-tanick.github.io/your-barcode-maker/

## 動かし方

1. STM32F030を使い、バーコードを読み取ります。
2. Aボタンを押すことでランダムな顔が生成されます。

その他のボタンの機能

- Bボタン：ボタンを押すごとに、各顔パーツの元ネタとなる顔を順番に表示します。10種類＋スペシャルの11種類が表示されます。
- Cボタン：サーボモーターがつながっている場合、5回ランダムに顔を上下させます。5回が終わったら初期位置に戻ります。

## Thanks

- m5stack-avatar @meganetaaan
  - 顔のベースとして利用したライブラリ
  - 顔のパーツ単位での選択を可能にするため、libフォルダ内にカスタマイズしたものを格納しています。
  - https://github.com/meganetaaan/m5stack-avatar/tree/master

- STM32F030でバーコードを読み取るために利用したライブラリ（M5UnitQRCode）
  - https://www.arduino.cc/reference/en/libraries/m5unitqrcode/

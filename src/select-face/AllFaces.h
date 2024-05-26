// Copyright (c) Ko Umetani. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.
#include <M5Unified.h>
#include <Avatar.h>

namespace m5avatar
{
  /**
   * 0 : BaseFace
   * 1 : AsciiFace
   * 2 : DanboFace
   * 3 : KappaFace
   * 
   * 4 : MaroFace
   * 5 : ChiikawaFace
   * 6 : SunglassFace
   * 7 : KenFace
   * 8 : Girl1Face
   * 9 : Other
  */

  class AllEye : public Drawable
  {

  private:
    uint16_t r;
    bool isLeft;

  public:
    AllEye(uint16_t r, bool isLeft)
        : r{r}, isLeft{isLeft} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx)
    {
      ColorPalette *cp = ctx->getColorPalette();
      uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
      uint16_t backgroundColor = ctx->getColorDepth() == 1 ? ERACER_COLOR : cp->get(COLOR_BACKGROUND);

      Expression exp = ctx->getExpression();
      uint32_t x = rect.getCenterX();
      uint32_t y = rect.getCenterY();
      Gaze g = ctx->getGaze();
      float openRatio = ctx->getEyeOpenRatio();
      uint32_t offsetX = g.getHorizontal() * 3;
      uint32_t offsetY = g.getVertical() * 3;

      int eyetype = ctx->getEyeType();
      if (eyetype == 0) {  // BaseFace
        if (openRatio > 0) {
          spi->fillCircle(x + offsetX, y + offsetY, r, primaryColor);
          // TODO(meganetaaan): Refactor
          if (exp == Expression::Angry || exp == Expression::Sad) {
            int x0, y0, x1, y1, x2, y2;
            x0 = x + offsetX - r;
            y0 = y + offsetY - r;
            x1 = x0 + r * 2;
            y1 = y0;
            x2 = !isLeft != !(exp == Expression::Sad) ? x0 : x1;
            y2 = y0 + r;
            spi->fillTriangle(x0, y0, x1, y1, x2, y2, backgroundColor);
          }
          if (exp == Expression::Happy || exp == Expression::Sleepy) {
            int x0, y0, w, h;
            x0 = x + offsetX - r;
            y0 = y + offsetY - r;
            w = r * 2 + 4;
            h = r + 2;
            if (exp == Expression::Happy) {
              y0 += r;
              spi->fillCircle(x + offsetX, y + offsetY, r / 1.5, backgroundColor);
            }
            spi->fillRect(x0, y0, w, h, backgroundColor);
          }
        } else {
          int x1 = x - r + offsetX;
          int y1 = y - 2 + offsetY;
          int w = r * 2;
          int h = 4;
          spi->fillRect(x1, y1, w, h, primaryColor);
        }
      } 
      else if (eyetype == 1) { // AsciiFace
        int x1 = x - 18 + offsetX;
        int y1 = y - 2 + offsetY;
        int x2 = x1;
        int y2 = y1 + 15;
        int w = 38;
        int h = 6;
        spi->fillRect(x1, y1, w, h, primaryColor);
        spi->fillRect(x2, y2, w, h, primaryColor);
      }
      else if (eyetype == 2) { // DanboFace
        // 目の位置とサイズ調整、左右の高さをそろえる修正
        int dnb_r = 18;
        if (isLeft == true) {
          y = y - 3;
        }
        y = y - 10 + dnb_r;

        if (openRatio > 0) {
          spi->fillCircle(x + offsetX, y + offsetY, dnb_r, primaryColor);
          // TODO(meganetaaan): Refactor
          if (exp == Expression::Angry || exp == Expression::Sad) {
            int x0, y0, x1, y1, x2, y2;
            x0 = x + offsetX - dnb_r;
            y0 = y + offsetY - dnb_r;
            x1 = x0 + dnb_r * 2;
            y1 = y0;
            x2 = !isLeft != !(exp == Expression::Sad) ? x0 : x1;
            y2 = y0 + dnb_r;
            spi->fillTriangle(x0, y0, x1, y1, x2, y2, backgroundColor);
          }
          if (exp == Expression::Happy || exp == Expression::Sleepy) {
            int x0, y0, w, h;
            x0 = x + offsetX - dnb_r;
            y0 = y + offsetY - dnb_r;
            w = dnb_r * 2 + 4;
            h = dnb_r + 2;
            if (exp == Expression::Happy) {
              y0 += dnb_r;
              spi->fillCircle(x + offsetX, y + offsetY, dnb_r / 1.5, backgroundColor);
            }
            spi->fillRect(x0, y0, w, h, backgroundColor);
          }
        } else {
          int x1 = x - dnb_r + offsetX;
          int y1 = y - 2 + offsetY;
          int w = dnb_r * 2;
          int h = 4;
          spi->fillRect(x1, y1, w, h, primaryColor);
        }
      }
      else if (eyetype == 3) { // KappaFace
        if (openRatio > 0) {
          // 目が開いているとき
          int x0 = x - r + offsetX;
          int y0 = y - 2 + offsetY;
          int w = r;
          int h = 44;
          spi->fillRect(x0, y0, w, h, primaryColor);
        } else {
          // 目をつむったとき
          int x1 = x - r*1.5 + offsetX;
          int y1 = y + 24 + offsetY;
          int w = r * 4;
          int h = 4;
          spi->fillRect(x1, y1, w, h, primaryColor);
        }
      }
      else if (eyetype == 4) { // MaroFace
      }
      else {
      }

    }
  };

  class AllEyeblow : public Drawable
  {

  private:
    uint16_t width;
    uint16_t height;
    bool isLeft;

  public:
    AllEyeblow(uint16_t w, uint16_t h, bool isLeft)
        : width{w}, height{h}, isLeft{isLeft} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx)
    {
      ColorPalette *cp = ctx->getColorPalette();
      uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
      uint16_t backgroundColor = ctx->getColorDepth() == 1 ? ERACER_COLOR : cp->get(COLOR_BACKGROUND);
      Expression exp = ctx->getExpression();
      uint32_t x = rect.getLeft();
      uint32_t y = rect.getTop();

      int eyeblowtype = ctx->getEyeBlowType();
      if (eyeblowtype == 0) {  // BaseFace
        /* Nothing */
      } 
      else if (eyeblowtype == 1) { // AsciiFace 1
        int eb_height = 3;
        int x1, y1, x2, y2, x3, y3, x4, y4;
        int a = isLeft ? -1 : 1;
        int dx = a * 3;
        int dy = a * 5;
        x1 = x - width / 2;
        x2 = x1 - dx;
        x4 = x + width / 2;
        x3 = x4 + dx;
        y1 = y - eb_height / 2 - dy;
        y2 = y + eb_height / 2 - dy;
        y3 = y - eb_height / 2 + dy;
        y4 = y + eb_height / 2 + dy;
        spi->fillTriangle(x1, y1, x2, y2, x3, y3, primaryColor);
        spi->fillTriangle(x2, y2, x3, y3, x4, y4, primaryColor);
      }
      else if (eyeblowtype == 2) { // AsciiFace 2
        int eb_height = 3;
        int x1, y1, x2, y2, x3, y3, x4, y4;
        int a = isLeft ? 1 : -1;
        int dx = a * 3;
        int dy = a * 5;
        x1 = x - width / 2;
        x2 = x1 - dx;
        x4 = x + width / 2;
        x3 = x4 + dx;
        y1 = y - eb_height / 2 - dy;
        y2 = y + eb_height / 2 - dy;
        y3 = y - eb_height / 2 + dy;
        y4 = y + eb_height / 2 + dy;
        spi->fillTriangle(x1, y1, x2, y2, x3, y3, primaryColor);
        spi->fillTriangle(x2, y2, x3, y3, x4, y4, primaryColor);
      }
      else if (eyeblowtype == 3) { // KappaFace
        int x1, y1, x2, y2, x3, y3;
        height = 30;
        y = y - 22;
        if (isLeft) {
          y = y - 4;
          x1 = x + width*0.6;
          height = height + 3;
        } else {
          x1 = x - width;
        }
        x2 = x - width*1.5;
        x3 = x + width*0.9;
        y1 = y;
        y2 = y + height;
        y3 = y + height;
        spi->fillTriangle(x1, y1, x2, y2, x3, y3, primaryColor);
      }
      else if (eyeblowtype == 4) { // MaroFace
      }
      else {
      }
    }
  };

  class AllMouth : public Drawable
  {
  private:
    uint16_t minWidth;
    uint16_t maxWidth;
    uint16_t minHeight;
    uint16_t maxHeight;

  public:
    AllMouth() : AllMouth(0, 0, 0, 0) {}
    AllMouth(uint16_t minWidth, uint16_t maxWidth, uint16_t minHeight,
            uint16_t maxHeight)
        : minWidth{minWidth},
          maxWidth{maxWidth},
          minHeight{minHeight},
          maxHeight{maxHeight} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx)
    {
      ColorPalette *cp = ctx->getColorPalette();
      uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
      uint16_t backgroundColor = ctx->getColorDepth() == 1 ? ERACER_COLOR : cp->get(COLOR_BACKGROUND);
      Expression exp = ctx->getExpression();
      float breath = fmin(1.0f, ctx->getBreath());
      float openRatio = ctx->getMouthOpenRatio();

      int mouthtype = ctx->getMouthType();
      if (mouthtype == 0) {  // BaseFace
        int h = minHeight + (maxHeight - minHeight) * openRatio;
        int w = minWidth + (maxWidth - minWidth) * (1 - openRatio);
        int x = rect.getLeft() - w / 2;
        int y = rect.getTop() - h / 2 + breath * 2;
        spi->fillRect(x, y, w, h, primaryColor);
      }
      else if (mouthtype == 1) { // AsciiFace
        int h = minHeight + (maxHeight - minHeight) * openRatio;
        int w = minWidth + (maxWidth - minWidth) * (1 - openRatio);
        int x = rect.getLeft();
        int y = rect.getTop();

        int x2 = x*0.71;
        // 口の ω 部分として2つの楕円を描画
        int e_rx = 140;
        int e_ry = y;
        int e_lx = 183;
        int e_ly = y;
        int e_ex = 25;
        int e_ey = 29;
        spi->fillEllipse(e_rx, e_ry, e_ex, e_ey, primaryColor);
        spi->fillEllipse(e_lx, e_ly, e_ex, e_ey, primaryColor);

        // 口の ω を形成するために、少し小さい背景色の楕円を被せる
        int over_e_rx = e_rx;
        int over_e_ry = e_ry;
        int over_e_lx = e_lx;
        int over_e_ly = e_ly;
        int over_e_wx = 18;
        int over_e_wy = 24;
        spi->fillEllipse(over_e_rx, over_e_ry, over_e_wx, over_e_wy, backgroundColor);
        spi->fillEllipse(over_e_lx, over_e_ly, over_e_wx, over_e_wy, backgroundColor);

        // 口の ω を形成するために、楕円の上部に背景色の四角を被せる
        int over_r1_rx = x2 - 5;
        int over_r1_ry = y - 40;
        int over_r1_rw = 105;
        int over_r1_rh = 30;
        spi->fillRect(over_r1_rx, over_r1_ry, over_r1_rw, over_r1_rh, backgroundColor);
        int over_r2_rx = x2 + 35;
        int over_r2_ry = y - 10;
        int over_r2_rw = 25;
        int over_r2_rh = 12;
        spi->fillRect(over_r2_rx, over_r2_ry, over_r2_rw, over_r2_rh, backgroundColor);
      }
      else if (mouthtype == 2) { // DanboFace
        int w = 53;
        int h = 33;
        int x = rect.getLeft() - w/2;
        int y = rect.getTop() + 42;
        spi->fillTriangle(x, y, x+w, y, x+w/2, y-h, primaryColor);
      }
      else if (mouthtype == 3) { // KappaFace
        int h = 25;
        int w = 64;
        int x = rect.getLeft() - 8;
        int y = rect.getTop() + 20;
        spi->fillEllipse(x, y, w, h, TFT_RED);

        int edge_x1_l = x-w-5;
        int edge_x2_l = x-w+7;
        int edge_x3_l = x-w+7;
        int edge_x1_r = x+w+5;
        int edge_x2_r = x+w-7;
        int edge_x3_r = x+w-7;
        int edge_y1 = y;
        int edge_y2 = y+h/2;
        int edge_y3 = y-h/2;
        spi->fillTriangle(edge_x1_l, edge_y1, edge_x2_l, edge_y2, edge_x3_l, edge_y3, TFT_RED);
        spi->fillTriangle(edge_x1_r, edge_y1, edge_x2_r, edge_y2, edge_x3_r, edge_y3, TFT_RED);

        spi->drawLine(edge_x1_l, edge_y1, edge_x1_r, edge_y1, TFT_BLACK);
        int offset_y = 14;
        int philtrum_w = 14;
        int philtrum_h = 20;
        int x1_ptm = x;
        int x2_ptm = x - philtrum_w;
        int x3_ptm = x + philtrum_w;
        int y1_ptm = y - offset_y;
        int y23_ptm = y - philtrum_w - offset_y;
        spi->fillTriangle(x1_ptm, y1_ptm, x2_ptm, y23_ptm, x3_ptm, y23_ptm, backgroundColor);
      }
      else if (mouthtype == 4) { // MaroFace
      }
      else {
      }
    }
  };

  /**
   * Faceクラスを継承した全ての顔パーツ描画用のクラス
  */
  class AllFaces : public Face
  {
  public:
    AllFaces()
        : Face(new AllMouth(50, 90, 4, 60), new BoundingRect(148, 163),
              new AllEye(8, false),         new BoundingRect(93, 90),
              new AllEye(8, true),          new BoundingRect(96, 230),
              new AllEyeblow(32, 0, false), new BoundingRect(67, 96),
              new AllEyeblow(32, 0, true),  new BoundingRect(72, 230)) {}
  };

} // namespace m5avatar

// Copyright (c) Ko Umetani. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.
#include <M5Unified.h>
#include <Avatar.h>

namespace m5avatar
{

  class SunglassEye : public Drawable
  {

  private:
    uint16_t r;
    bool isLeft;

  public:
    SunglassEye(uint16_t r, bool isLeft)
        : r{r}, isLeft{isLeft} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx)
    {
      Expression exp = ctx->getExpression();
      uint32_t x = rect.getCenterX();
      uint32_t y = rect.getCenterY();
      Gaze g = ctx->getGaze();
      float openRatio = ctx->getEyeOpenRatio();
      uint32_t offsetX = g.getHorizontal() * 3;
      uint32_t offsetY = g.getVertical() * 3;
      uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : ctx->getColorPalette()->get(COLOR_PRIMARY);
      uint16_t backgroundColor = ctx->getColorDepth() == 1 ? 0 : ctx->getColorPalette()->get(COLOR_BACKGROUND);

      int fix_pos_y = isLeft? -3 : 0;
      int lens_x = x - 55;
      int lens_y = y - 25 + fix_pos_y;
      int lens_w = 105;
      int lens_h = 70;
      int lens_r = 20;
      spi->fillRoundRect(lens_x, lens_y, lens_w, lens_h, lens_r, TFT_NAVY);

      int temple_xr = x + 45;
      int temple_yr = y - 10 + fix_pos_y;
      int temple_wr = 50;
      int temple_hr = 20;
      int temple_xl = x - 90;
      int temple_yl = y - 10 + fix_pos_y;
      int temple_wl = 35;
      int temple_hl = 20;
      spi->fillRect(temple_xr, temple_yr, temple_wr, temple_hr, TFT_NAVY);
      spi->fillRect(temple_xl, temple_yl, temple_wl, temple_hl, TFT_NAVY);

      int reflect_x_top1 = x - 10;
      int reflect_x_top2 = reflect_x_top1 + 25;
      int reflect_x_btm1 = x - 33;
      int reflect_x_btm2 = reflect_x_btm1 + 25;
      int reflect_y_top = y - 25 + fix_pos_y;
      int reflect_y_btm = y + 44 + fix_pos_y;
      uint16_t reflectColor = 0;
      reflectColor = isLeft ? M5.Lcd.color565(160,246,253) : M5.Lcd.color565(255,180,220);
      spi->fillTriangle(reflect_x_top1, reflect_y_top, reflect_x_top2, reflect_y_top, reflect_x_btm1, reflect_y_btm, reflectColor);
      spi->fillTriangle(reflect_x_top2, reflect_y_top, reflect_x_btm1, reflect_y_btm, reflect_x_btm2, reflect_y_btm, reflectColor);
    }
  };

  class SunglassEyeblow : public Drawable
  {

  private:
    uint16_t width;
    uint16_t height;
    bool isLeft;

  public:
    SunglassEyeblow(uint16_t w, uint16_t h, bool isLeft)
        : width{w}, height{h}, isLeft{isLeft} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx)
    {
      Expression exp = ctx->getExpression();
      uint32_t x = rect.getLeft();
      uint32_t y = rect.getTop();
      uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : ctx->getColorPalette()->get(COLOR_PRIMARY);

      int x0 = x;
      int y0 = isLeft ? y - 2: y + 2;
      spi->fillEllipseArc(x0, y0 - 20, 34, 40, 24, 26, 200, 340, primaryColor);
    }
  };

  class SunglassMouth : public Drawable
  {
  private:
    uint16_t minWidth;
    uint16_t maxWidth;
    uint16_t minHeight;
    uint16_t maxHeight;

  public:
    SunglassMouth() : SunglassMouth(0, 0, 0, 0) {}
    SunglassMouth(uint16_t minWidth, uint16_t maxWidth, uint16_t minHeight,
            uint16_t maxHeight)
        : minWidth{minWidth},
          maxWidth{maxWidth},
          minHeight{minHeight},
          maxHeight{maxHeight} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx)
    {
      uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : ctx->getColorPalette()->get(COLOR_PRIMARY);
      float breath = fmin(1.0f, ctx->getBreath());
      float openRatio = ctx->getMouthOpenRatio();

      int x0 = rect.getLeft();
      int y0 = rect.getTop() + 15;
      int rx1 = 0;
      int rx2 = 26;
      int ry1 = 0;
      int ry2 = 40 - (1 - openRatio)*20;
      spi->fillEllipseArc(x0, y0, rx1, rx2, ry1, ry2, 0, 180, primaryColor);
      int x1 = x0;
      int y1 = y0 + 28 - (1 - openRatio)*12;
      int rxt = 15;
      int ryt = 15 - (1 - openRatio)*3;
      spi->fillEllipse(x1, y1, rxt, ryt, TFT_RED);
    }
  };

  /**
   * Faceクラスを継承したSunglass顔描画用のクラス
  */
  class SunglassFace : public Face
  {
  public:
    SunglassFace()
        : Face(new SunglassMouth(50, 90, 4, 60), new BoundingRect(148, 163),
              new SunglassEye(8, false),         new BoundingRect(93, 90),
              new SunglassEye(8, true),          new BoundingRect(96, 230),
              new SunglassEyeblow(32, 0, false), new BoundingRect(67, 96),
              new SunglassEyeblow(32, 0, true),  new BoundingRect(72, 230)) {}
  };

} // namespace m5avatar
#include "core/draw.h"

#include "core/numeric.h"
#include "core/assert.h"
#include "core/range.h"

#include <font8x8/font8x8_basic.h>

#include <utility>

Draw::Draw(Image* image)
    : image_(image)
{
  ASSERT(image);
}

Recti
Draw::WholeImage() const
{
  return Recti::FromTopLeftWidthHeight(
      image_->GetHeight(), 0, image_->GetWidth(), image_->GetHeight());
}

Draw&
Draw::Clear(const Rgbi& color)
{
  return Rect(color, WholeImage());
}

Draw&
Draw::Rect(const Rgbi& color, const Recti& rect)
{
  const int left   = rect.TopLeft().x;
  const int right  = rect.TopRight().x;
  const int top    = rect.TopLeft().y;
  const int bottom = rect.BottomLeft().y;
  // ASSERTX(left >= 0, left);
  // ASSERTX(bottom >= 0, bottom);
  for(int y = bottom; y < top; ++y)
  {
    if(y < 0 || y >= image_->GetHeight() ) continue;
    for(int x = left; x < right; ++x)
    {
      if(x < 0 || x >= image_->GetWidth() ) continue;
      image_->SetPixel(x, y, color);
    }
  }

  return *this;
}


Draw&
Draw::Square(const Rgbi& color, int x, int y, int size)
{
  // is the +1 right?
  return Rect(color, Recti::FromTopLeftWidthHeight(y + 1, x, size, size));
}

Draw&
Draw::Circle(
    const Rgb&     color,
    const point2i& center,
    float          radius,
    float          softness,
    float          inner)
{
  const int left = Max(0, Floori(center.x - radius - softness));
  const int right =
      Min(image_->GetWidth(), Ceili(center.x + radius + softness));
  const int top = Max(0, Floori(center.y - radius - softness));
  const int bottom =
      Min(image_->GetHeight(), Ceili(center.y + radius + softness));

  // color modes
  // nothing INNER-SOFTNESS fade INNER full RADIUS fade RADIUS+SOFTNESS nothing

  for(int y = top; y < bottom; ++y)
  {
    for(int x = left; x < right; ++x)
    {
      // todo: use length squared!
      const float sq =
          vec2f::FromTo(
              point2f{static_cast<float>(x), static_cast<float>(y)},
              center.StaticCast<float>())
              .GetLength();
      bool  blend        = false;
      float blend_factor = 1.0f;

      Range a{inner - softness, inner};
      Range b{radius, radius + softness};

      if(a.IsWithin(sq))
      {
        blend_factor = a.To01(sq);
        blend        = true;
      }
      else if(b.IsWithin(sq))
      {
        blend_factor = 1.0f - b.To01(sq);
        blend        = true;
      }
      else if(Range{inner, radius}.IsWithin(sq))
      {
        // full color
      }
      else
      {
        // outside
        continue;
      }

      const Rgb paint_color =
          blend ? RgbTransform::Transform(
                      rgb(image_->GetPixel(x, y)), blend_factor, color)
                : color;

      image_->SetPixel(x, y, Rgbi{paint_color});
    }
  }

  return *this;
}

Draw&
Draw::LineFast(const Rgbi& color, const point2i& from, const point2i& to)
{
  // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  const int x0 = from.x;
  const int y0 = from.y;
  const int x1 = to.x;
  const int y1 = to.y;

  int         deltax = x1 - x0;
  int         deltay = y1 - y0;
  const float deltaerr =
      deltax == 0 ? -1 : Abs(deltay / static_cast<float>(deltax));
  float error = deltaerr - 0.5f;
  int   y     = y0;
  for(int x = x0; x < x1; x += 1)
  {
    const bool valid_x = IsWithinInclusivei(0, x, image_->GetWidth() - 1);
    const bool valid_y = IsWithinInclusivei(0, y, image_->GetHeight() - 1);
    if(valid_x && valid_y)
    {
      image_->SetPixel(x, y, color);
    }

    if(deltaerr > 0)
    {
      error = error + deltaerr;
      if(error >= 0.5)
      {
        if(deltay > 0)
        {
          y = y + 1;
        }
        else
        {
          y = y - 1;
        }
      }
      error = error - 1.0;
    }
  }

  return *this;
}

int
Round(float x)
{
  return Floori(x + 0.5f);
}

// fractional part of x
float
GetFractionalPart(float x)
{
  return x - Floor(x);
}

float
GetOneMinusFractionalPart(float x)
{
  return 1.0f - GetFractionalPart(x);
}

void
Plot(int x, int y, float brightness, const Rgb& color, Image* image)
{
  const bool valid_x = IsWithinInclusivei(0, x, image->GetWidth() - 1);
  const bool valid_y = IsWithinInclusivei(0, y, image->GetHeight() - 1);
  if(valid_x && valid_y)
  {
    const Rgb paint_color =
        RgbTransform::Transform(rgb(image->GetPixel(x, y)), brightness, color);
    image->SetPixel(x, y, Rgbi{paint_color});
  }
}

Draw&
Draw::LineAntialiased(const Rgb& color, const point2i& from, const point2i& to)
{
  return LineAntialiased(
      color, from.StaticCast<float>(), to.StaticCast<float>());
}

Draw&
Draw::LineAntialiased(const Rgb& color, const point2f& from, const point2f& to)
{
  // https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
  float x0 = from.x;
  float y0 = from.y;
  float x1 = to.x;
  float y1 = to.y;


  bool steep = Abs(y1 - y0) > Abs(x1 - x0);

  if(steep)
  {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }
  if(x0 > x1)
  {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  float dx       = x1 - x0;
  float dy       = y1 - y0;
  float gradient = dx == 0.0 ? 1.0 : dy / dx;

  // handle first endpoint
  int   xend  = Round(x0);
  float yend  = y0 + gradient * (xend - x0);
  float xgap  = GetOneMinusFractionalPart(x0 + 0.5);
  int   xpxl1 = xend;  // this will be used in the main loop
  float ypxl1 = Floori(yend);
  if(steep)
  {
    Plot(ypxl1, xpxl1, GetOneMinusFractionalPart(yend) * xgap, color, image_);
    Plot(ypxl1 + 1, xpxl1, GetFractionalPart(yend) * xgap, color, image_);
  }
  else
  {
    Plot(xpxl1, ypxl1, GetOneMinusFractionalPart(yend) * xgap, color, image_);
    Plot(xpxl1, ypxl1 + 1, GetFractionalPart(yend) * xgap, color, image_);
  }
  int intery = yend + gradient;  // first y-intersection for the main loop

  // handle second endpoint
  xend      = Round(x1);
  yend      = y1 + gradient * (xend - x1);
  xgap      = GetFractionalPart(x1 + 0.5);
  int xpxl2 = xend;  // this will be used in the main loop
  int ypxl2 = Floori(yend);
  if(steep)
  {
    Plot(ypxl2, xpxl2, GetOneMinusFractionalPart(yend) * xgap, color, image_);
    Plot(ypxl2 + 1, xpxl2, GetFractionalPart(yend) * xgap, color, image_);
  }
  else
  {
    Plot(xpxl2, ypxl2, GetOneMinusFractionalPart(yend) * xgap, color, image_);
    Plot(xpxl2, ypxl2 + 1, GetFractionalPart(yend) * xgap, color, image_);
  }

  // main loop
  if(steep)
  {
    for(int x = xpxl1 + 1; x < xpxl2 - 1; x += 1)
    {
      Plot(Floori(intery), x, GetOneMinusFractionalPart(intery), color, image_);
      Plot(Floori(intery) + 1, x, GetFractionalPart(intery), color, image_);
      intery = intery + gradient;
    }
  }
  else
  {
    for(int x = xpxl1 + 1; x < xpxl2 - 1; x += 1)
    {
      Plot(x, Floori(intery), GetOneMinusFractionalPart(intery), color, image_);
      Plot(x, Floori(intery) + 1, GetFractionalPart(intery), color, image_);
      intery = intery + gradient;
    }
  }
  return *this;
}

const unsigned char*
GetCharGlyph(char ac)
{
  auto c = static_cast<int>(ac);
  ASSERT(c >= 0);
  ASSERT(c < 128);
  return static_cast<const unsigned char*>(font8x8_basic[c]);
}

void
PrintCharAt(
    Draw* image, const point2i pos, char c, const Rgbi& color, int scale)
{
  ASSERT(image);
  const unsigned char* glyph = GetCharGlyph(c);
  for(int y = 0; y < 8; y += 1)
  {
    for(int x = 0; x < 8; x += 1)
    {
      // extract pixel from character
      // glyph is defined in "y down" order, fix by inverting sample point on y
      bool pixel = 0 != (glyph[7 - y] & 1 << x);
      if(pixel)
      {
        image->Square(color, pos.x + x * scale, pos.y + y * scale, scale);
        // image->SetPixel(pos.x+x*scale, pos.y + y*scale, color);
      }
    }
  }
}

Draw&
Draw::Text(
    const point2i&     start_pos,
    const std::string& text,
    const Rgbi&        color,
    int                scale)
{
  ASSERT(scale > 0);

  point2i pos = start_pos;
  for(unsigned int i = 0; i < text.length(); i += 1)
  {
    const char c = text[i];

    if(pos.x + 8 * scale > image_->GetWidth())
    {
      pos.x = start_pos.x;
      pos.y -= 8 * scale;
    }

    PrintCharAt(this, pos, c, color, scale);
    pos.x += 8 * scale;  // move to next char
  }

  return *this;
}

Draw&
Draw::PasteImage(const point2i& position, const Image& source_image)
{
  for(int y = 0; y < source_image.GetHeight(); ++y)
  {
    for(int x = 0; x < source_image.GetWidth(); ++x)
    {
      image_->SetPixel(
          position.x + x, position.y + y, source_image.GetPixel(x, y));
    }
  }
  return *this;
}

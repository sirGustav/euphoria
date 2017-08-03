#include "draw.h"

#include "core/assert.h"

Draw::Draw(Image* image)
: image_(image)
{
  Assert(image);
}

Recti Draw::WholeImage() const
{
  return Recti::FromTopLeftWidthHeight(0, 0, image_->GetWidth(), image_->GetHeight());
}

Draw& Draw::Clear(const Rgb &color)
{
  return Square(color, WholeImage());
}

Draw& Draw::Square(const Rgb& color, const Recti& rect)
{
  const int left = rect.TopLeft().x;
  const int right = rect.TopRight().x;
  const int top = rect.TopLeft().y;
  const int bottom = rect.BottomLeft().y;
  for(int y=top; y<bottom; ++y) {
    for(int x=left; x<right; ++x) {
      image_->SetPixel(x, y, color);
    }
  }

  return *this;
}

Draw& Draw::Circle(const Rgb& color, const vec2i& center, float radius, float softness, float inner)
{
  const int left =  Max(0, floor(center.x - radius - softness));
  const int right = Min(image_->GetWidth(), ceil(center.x + radius + softness));
  const int top = Max(0, floor(center.y - radius - softness));
  const int bottom = Min(image_->GetWidth(), ceil(center.x + radius + softness));

  // color modes
  // nothing INNER-SOFTNESS fade INNER full RADIUS fade RADIUS+SOFTNESS nothing

  for(int y=top; y<bottom; ++y) {
    for(int x=left; x<right; ++x) {
      // todo: use length squared!
      const float sq = vec2f::FromTo(vec2f{static_cast<float>(x),static_cast<float>(y)},
                                     vec2f{static_cast<float>(center.x), static_cast<float>(center.y)}).GetLength();
      bool blend = false;
      float blend_factor = 1.0f;

      if( IsWithinInclusive(inner-softness, sq, inner) ) {
        blend_factor = From01(inner-softness, sq, inner);
        blend = true;
      }
      else if (IsWithinInclusive(radius, sq, radius+softness)) {
        blend_factor = 1.0f - From01(radius, sq, radius+softness);
        blend = true;
      }
      else if(IsWithinInclusive(inner, sq, radius)) {
        // full color
      }
      else {
        // outside
        continue;
      }

      const Rgb paint_color =
          blend
          ? RgbTransform::Transform(Rgb{image_->GetPixel(x,y)}, blend_factor, color)
          : color;

      image_->SetPixel(x, y, paint_color);
    }
  }

  return *this;
}

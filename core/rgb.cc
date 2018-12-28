#include "core/rgb.h"
#include "core/interpolate.h"
#include "core/numeric.h"
#include "core/stringutils.h"
#include <iostream>
#include <map>

////////////////////////////////////////////////////////////////////////////////

Rgbi::Rgbi(unsigned char red, unsigned char green, unsigned char blue)
    : r(red)
    , g(green)
    , b(blue)
{
}

Rgbi::Rgbi(unsigned char gray)
    : r(gray)
    , g(gray)
    , b(gray)
{
}

Rgbi::Rgbi(Color color)
    : Rgbi(Rgbi::FromHex(ToColorHex(color)))
{
}

Rgbi::Rgbi(const Rgb& rgb)
    : r(colorutil::ToUnsignedChar(rgb.r))
    , g(colorutil::ToUnsignedChar(rgb.g))
    , b(colorutil::ToUnsignedChar(rgb.b))
{
}

Rgbi
Rgbi::FromHex(unsigned int hex)
{
  return {colorutil::GetRed(hex),
          colorutil::GetGreen(hex),
          colorutil::GetBlue(hex)};
}


////////////////////////////////////////////////////////////////////////////////

Rgbai::Rgbai(const Rgbi& rgb, unsigned char alpha)
    : r(rgb.r)
    , g(rgb.g)
    , b(rgb.b)
    , a(alpha)
{
}

Rgbai::Rgbai(const Rgba& rgba)
    : r(colorutil::ToUnsignedChar(rgba.r))
    , g(colorutil::ToUnsignedChar(rgba.g))
    , b(colorutil::ToUnsignedChar(rgba.b))
    , a(colorutil::ToUnsignedChar(rgba.a))
{
}

////////////////////////////////////////////////////////////////////////////////

Rgb::Rgb(float red, float green, float blue)
    : r(red)
    , g(green)
    , b(blue)
{
}

Rgb::Rgb(float gray)
    : r(gray)
    , g(gray)
    , b(gray)
{
}

Rgb::Rgb(Color color)
    : Rgb(Rgb::FromHex(ToColorHex(color)))
{
}

Rgb
Rgb::FromHex(unsigned int hex)
{
  const auto b = colorutil::ToFloat(colorutil::GetBlue(hex));
  const auto g = colorutil::ToFloat(colorutil::GetGreen(hex));
  const auto r = colorutil::ToFloat(colorutil::GetRed(hex));
  return Rgb{r, g, b};
}

////////////////////////////////////////////////////////////////////////////////

Rgba::Rgba(const Rgb& rgb, float alpha)
    : r(rgb.r)
    , g(rgb.g)
    , b(rgb.b)
    , a(alpha)
{
}

////////////////////////////////////////////////////////////////////////////////

#define IV(x) static_cast<int>(x)

std::ostream&
operator<<(std::ostream& stream, const Rgbi& v)
{
  return stream << "(" << IV(v.r) << ", " << IV(v.g) << ", " << IV(v.b) << ")";
}


std::ostream&
operator<<(std::ostream& stream, const Rgbai& v)
{
  return stream << "(" << IV(v.r) << ", " << IV(v.g) << ", " << IV(v.b) << ", "
                << IV(v.a) << ")";
}

#undef IV

std::ostream&
operator<<(std::ostream& stream, const Rgb& v)
{
  return stream << "(" << v.r << ", " << v.g << ", " << v.b << ")";
}


std::ostream&
operator<<(std::ostream& stream, const Rgba& v)
{
  return stream << "(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a
                << ")";
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Default compare

bool
operator==(const Rgbai& lhs, const Rgbai& rhs)
{
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

////////////////////////////////////////////////////////////////////////////////

Rgb
rgb(const Rgbi& rgb)
{
  return {colorutil::ToFloat(rgb.r),
          colorutil::ToFloat(rgb.g),
          colorutil::ToFloat(rgb.b)};
}

Rgb
rgb(const Rgba& rgb)
{
  return {rgb.r, rgb.g, rgb.b};
}

Rgb
rgb(const Rgbai& rgb)
{
  return {colorutil::ToFloat(rgb.r),
          colorutil::ToFloat(rgb.g),
          colorutil::ToFloat(rgb.b)};
}

Rgb
rgb(DawnbringerPalette color)
{
  return {Rgb::FromHex(ToColorHex(color))};
}


// Convert functions (rgbi)

Rgbi
rgbi(const Rgb& rgb)
{
  return {colorutil::ToUnsignedChar(rgb.r),
          colorutil::ToUnsignedChar(rgb.g),
          colorutil::ToUnsignedChar(rgb.b)};
}

Rgbi
rgbi(const Rgba& rgb)
{
  return {colorutil::ToUnsignedChar(rgb.r),
          colorutil::ToUnsignedChar(rgb.g),
          colorutil::ToUnsignedChar(rgb.b)};
}

Rgbi
rgbi(const Rgbai& rgb)
{
  return {rgb.r, rgb.g, rgb.b};
}

Rgbi
rgbi(DawnbringerPalette color)
{
  return Rgbi::FromHex(ToColorHex(color));
}

////////////////////////////////////////////////////////////////////////////////

Rgb
RgbTransform::Transform(const Rgb& from, float v, const Rgb& to)
{
  return {FloatTransform::Transform(from.r, v, to.r),
          FloatTransform::Transform(from.g, v, to.g),
          FloatTransform::Transform(from.b, v, to.b)};
}

////////////////////////////////////////////////////////////////////////////////

namespace colorutil
{
  const unsigned char
  GetComponent(unsigned int i, int steps)
  {
    const int value = ((i >> 8 * steps) & 0xff);
    ASSERTX(IsWithinInclusivei(0, value, 255), value);
    return static_cast<unsigned char>(value);
  }

  const unsigned char
  GetRed(unsigned int rgb)
  {
    return GetComponent(rgb, 2);
  }

  const unsigned char
  GetGreen(unsigned int rgb)
  {
    return GetComponent(rgb, 1);
  }

  const unsigned char
  GetBlue(unsigned int rgb)
  {
    return GetComponent(rgb, 0);
  }

  float
  ToFloat(unsigned char c)
  {
    return c / 255.0f;
  }

  unsigned char
  ToUnsignedChar(float f)
  {
    return static_cast<unsigned char>(f * 255.0f);
  }

  unsigned int
  FromStringToHex(const std::string& str)
  {
    auto s = Trim(str);
    ASSERT(!s.empty());
    s = s[0] == '#' ? ToLower(s.substr(1)) : ToLower(s);
    if(s.length() == 3)
    {
      s = Str() << s[0] << s[0] << s[1] << s[1] << s[2] << s[2];
    }
    if(s.length() != 6)
      return 0;
    std::istringstream ss{s};
    unsigned int       hex = 0;
    ss >> std::hex >> hex;
    ASSERTX(!ss.fail(), s, str, hex);
    return hex;
  }
}  // namespace colorutil

#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include <string>
#include <vector>
#include "core/rgb.h"

class Image {
 public:
  void MakeInvalid();
  bool IsValid() const;

  void Setup(int image_width, int image_height, bool alpha, int default_value=0);

  void SetPixel(int x, int y, const Rgb& color);
  void SetPixel(int x, int y, const Rgba& color);
  void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

  Rgba GetPixel(int x, int y) const;

  int GetWidth() const;
  int GetHeight() const;
  bool HasAlpha() const;

  const unsigned char* GetPixelData() const;

 private:
  int GetPixelByteSize() const;
  unsigned long GetPixelIndex(int x, int y) const;

  std::vector<unsigned char> components;
  int width_;
  int height_;
  bool has_alpha_;
};

class ImageLoadResult {
 public:
  Image image;

  std::string error;
};

enum class AlphaLoad { Remove, Include };

// todo: move image loading to a io library instead
ImageLoadResult LoadImage(const std::string& path, AlphaLoad alpha);

#endif  // CORE_IMAGE_H

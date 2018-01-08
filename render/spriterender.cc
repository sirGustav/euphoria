#include "render/spriterender.h"
#include "render/buffer2d.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/gl.h"
#include "render/bufferbuilder2d.h"

//////////////////////////////////////////////////////////////////////////

DrawData&
DrawData::Rotation(const Angle& r)
{
  rotation = r;
  return *this;
}

DrawData&
DrawData::Scale(const vec2f& s)
{
  scale = s;
  return *this;
}

DrawData&
DrawData::Tint(const Rgba& t)
{
  tint = t;
  return *this;
}

DrawData&
DrawData::Anchor(const vec2f& a)
{
  anchor = a;
  return *this;
}

//////////////////////////////////////////////////////////////////////////

SpriteRenderer::SpriteRenderer(Shader* shader)
    : shader_(shader)
    , color_(shader->GetUniform("color"))
    , model_(shader->GetUniform("model"))
{
  shader_ = shader;
  InitRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
  buffer_.reset();
}

void
SpriteRenderer::DrawRect(
    const Texture2d& texture,
    const Rectf&     sprite_area,
    const Rectf&     texture_region,
    const Angle&     rotation_angle,
    const vec2f&     rotation_anchor,
    const Rgba&      tint_color)
{
  Use(shader_);
  vec3f rotation_anchor_displacement{
      -rotation_anchor.x, rotation_anchor.y - 1, 0.0f};
  const mat4f model =
      mat4f::Identity()
          .Translate(vec3f(sprite_area.BottomLeft(), 0.0f))
          .Scale(vec3f{sprite_area.GetWidth(), sprite_area.GetHeight(), 1.0f})
          .Translate(-rotation_anchor_displacement)
          .Rotate(AxisAngle::RightHandAround(
              vec3f::ZAxis(),
              rotation_angle))  // rotate around center
          .Translate(rotation_anchor_displacement);

  shader_->SetUniform(model_, model);
  shader_->SetUniform(color_, tint_color);

  glActiveTexture(GL_TEXTURE0);
  Use(&texture);
  buffer_->Draw();
}

void
SpriteRenderer::DrawSprite(
    const Texture2d& texture, const vec2f& position, const DrawData& data)
{
  DrawRect(
      texture,
      Rectf::FromPositionAnchorWidthAndHeight(
          position, data.anchor, texture.GetWidth(), texture.GetHeight()),
      Rectf::FromTopLeftWidthHeight(1, 0, 1, 1),
      data.rotation,
      vec2f{0.5f, 0.5f},
      data.tint);
}

void
SpriteRenderer::CommonDraw(const vec2f& position, const DrawData& data) const
{
  Use(shader_);
  const mat4f model =
      mat4f::Identity()
          .Translate(vec3f(position, 0.0f))
          .Rotate(AxisAngle::RightHandAround(
              vec3f::ZAxis(),
              data.rotation))  // rotate around center
          .Scale(vec3f(data.scale, 1.0f))
          .Translate(vec3f(-data.anchor.x, data.anchor.y - 1, 0.0f));

  shader_->SetUniform(model_, model);
  shader_->SetUniform(color_, data.tint);
}

void
SpriteRenderer::DrawNinepatch(
    const ScalableSprite& ninepatch,
    const vec2f&          position,
    const DrawData&       data)
{
  // const vec2f size = scale;
  const auto  size = ninepatch.GetSize();
  const auto  half = size / 2.0f;
  const vec2f d(
      half.GetWidth() * (data.scale.x - 1.0f),
      half.GetHeight() * (data.scale.y - 1.0f));
  // todo: always moving up by height, change this in the buffer instead
  CommonDraw(position - d - vec2f{0, size.GetHeight()}, data);

  glActiveTexture(GL_TEXTURE0);
  Use(ninepatch.GetTextureId());

  ninepatch.GetBufferPtr()->Draw();  // todo: fix this
  // vao_->Draw();
}

void
SpriteRenderer::InitRenderData()
{
  BufferBuilder2d data;

  Point a(0.0f, 1.0f, 0.0f, 0.0f);
  Point b(1.0f, 0.0f, 1.0f, 1.0f);
  Point c(0.0f, 0.0f, 0.0f, 1.0f);
  Point d(1.0f, 1.0f, 1.0f, 0.0f);

  data.AddQuad(c, b, a, d);

  buffer_ = std::make_unique<Buffer2d>(data);
}

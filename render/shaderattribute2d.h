#ifndef RENDER_SHADERATTRIBUTE2D_H
#define RENDER_SHADERATTRIBUTE2D_H

#include "render/shaderattribute.h"

namespace euphoria::render
{
    class Shader;

    namespace attributes2d
    {
        const ShaderAttribute&
        Vertex();

        void
        PrebindShader(Shader* shader);
    }  // namespace attributes2d

}  // namespace euphoria::render

#endif  // RENDER_SHADERATTRIBUTE2D_H

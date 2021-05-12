#include "core/vfs_defaultshaders.h"

#include "core/stringutils.h"
#include "core/log.h"
#include "core/vfs_path.h"

namespace euphoria::core::vfs
{
    void
    add_default_shaders(file_system* fs, const dir_path& base)
    {
        auto cat = read_root_catalog::create_and_add(fs);
        cat->register_file_string(
                base.get_file("sprite.vert"),
                R"STRING(
                #version 330 core

                in vec4 vertex; // <vec2 position, vec2 texCoords>

                out vec2 TexCoords;

                uniform mat4 model;
                uniform mat4 projection;

                uniform vec4 color;
                uniform vec4 region;

                void main()
                {
                    TexCoords = vec2(region.x + vertex.z * (region.y - region.x),
                                    region.z + vertex.w * (region.w - region.z));
                    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
                }
            )STRING");
        cat->register_file_string(
                base.get_file("sprite.frag"),
                R"STRING(
                #version 330 core

                in vec2 TexCoords;
                out vec4 outColor;

                uniform sampler2D image;
                uniform vec4 color;

                void main()
                {
                    outColor = color * texture(image, TexCoords);
                }
            )STRING");
    }
}

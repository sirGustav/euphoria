#ifndef EUPHORIA_MATERIALSHADER_H
#define EUPHORIA_MATERIALSHADER_H

#include <vector>

#include "core/enum.h"
#include "render/shader.h"

namespace euphoria::render
{
    class Light;

    class MaterialShaderDefaultTexture
    {
    public:
        MaterialShaderDefaultTexture(
                const core::EnumValue& name,
                const std::string&     path);

        const core::EnumValue&
        GetName() const;

        const std::string&
        GetPath() const;

    private:
        core::EnumValue name_;
        std::string     path_;
    };

    class MaterialShaderBinding
    {
    public:
        MaterialShaderBinding(
                ShaderUniform          uniform,
                const core::EnumValue& name);

        const ShaderUniform&
        GetUniform() const;

        const core::EnumValue&
        GetName() const;

    private:
        ShaderUniform   uniform_;
        core::EnumValue name_;
    };

    class MaterialShader
    {
    public:
        MaterialShader();

        // todo: add default textures, so we can optionally bind a white texture
        // to Diffuse if Diffuse is missing on the material

        bool
        Load(core::vfs::FileSystem* file_system, const std::string& path);

        bool
        Compile(const glchar* vertex,
                const glchar* fragment,
                const glchar* geom = nullptr);

        void
        UseShader();

        void
        SetProjection(const core::mat4f& projection);

        void
        SetView(const core::mat4f& view);

        void
        SetModel(const core::mat4f& model);

        void
        SetupLight(const Light& light, const core::vec3f& camera);

        void
        SetColors(
                const core::Rgb& ambient,
                const core::Rgb& diffuse,
                const core::Rgb& specular,
                float            shininess);

        const std::vector<MaterialShaderBinding>&
        GetBindings() const;

        const std::vector<MaterialShaderDefaultTexture>&
        GetDefaultTextures();

        Shader shader_;

        // private:
        ShaderUniform projection_;
        ShaderUniform view_;
        ShaderUniform model_;

        ShaderUniform ambient_;
        ShaderUniform diffuse_;
        ShaderUniform specular_;
        ShaderUniform shininess_;

        bool          hasLight_;
        ShaderUniform lightAmbient_;
        ShaderUniform lightDiffuse_;
        ShaderUniform lightSpecular_;
        ShaderUniform lightPosition_;
        ShaderUniform lightDirection_;
        ShaderUniform lightType_;
        ShaderUniform lightAttenuationConstant_;
        ShaderUniform lightAttenuationLinear_;
        ShaderUniform lightAttenuationQuadratic_;
        ShaderUniform lightCutoffAngleOuter_;
        ShaderUniform lightCutoffAngleInner_;

        ShaderUniform normalMatrix_;
        ShaderUniform viewPosition_;

        std::vector<MaterialShaderBinding>        bindings_;
        std::vector<MaterialShaderDefaultTexture> default_textures_;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_MATERIALSHADER_H

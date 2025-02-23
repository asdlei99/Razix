#pragma once

#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZVertexBufferLayout.h"

// TODO: Check spirv includes order in the razix codebase to get rid of SPV_REVISION redefinition warning
#include <SPIRVCross/spirv_glsl.hpp>

namespace Razix {
    namespace Graphics {

        class OpenGLShader : public RZShader
        {
        public:
            OpenGLShader(const std::string& filePath);
            ~OpenGLShader();

            void Bind() const override;
            void Unbind() const override;
            void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) override;

            void pushTypeToBuffer(const spirv_cross::SPIRType type, Graphics::RZVertexBufferLayout& layout, const std::string& name);

            u32 getProgramID() { return m_ProgramID; }

            void DestroyResource() override;

        private:
            u32                          m_ProgramID; /* OpenGL ID for the shader program that will bind to the pipeline  */
            RZVertexBufferLayout         m_Layout;    /* The layout of the vertex buffer and it's attributes              */
            std::vector<RZDescriptorSet> m_Sets;      /* The uniform and sampler binding layout information */

        private:
            void init();
        };
    }    // namespace Graphics
}    // namespace Razix

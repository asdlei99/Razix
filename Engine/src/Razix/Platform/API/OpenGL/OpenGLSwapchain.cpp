// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLSwapchain.h"

#ifdef RAZIX_RENDER_API_OPENGL

    #include "Razix/Platform/API/OpenGL/OpenGLContext.h"

    #include <glfw/glfw3.h>

namespace Razix {
    namespace Graphics {

        OpenGLSwapchain::OpenGLSwapchain(u32 width, u32 height)
            : m_Width(width), m_Height(height) 
        {
            m_DummyCmdBuffer = new OpenGLCommandBuffer();
        }

        void OpenGLSwapchain::Flip()
        {
            glfwSwapBuffers(OpenGLContext::Get()->getGLFWWindow());
        }
    }    // namespace Graphics
}    // namespace Razix
#endif
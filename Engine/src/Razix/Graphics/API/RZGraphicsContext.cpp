#include "rzxpch.h"
#include "RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Platform/api/OpenGL/OpenGLContext.h"
#endif 

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKContext.h"
#endif

#ifdef RAZIX_RENDER_API_METAL
#include "Razix/Platform/API/Metal/MTLContext.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX11
#include "Razix/Platform/API/DirectX11/DX11Context.h"
#endif

namespace Razix {

    namespace Graphics {

        // Initializing the static variables
        RZGraphicsContext* RZGraphicsContext::s_Context = nullptr;
        // The Engine uses Vulkan aS the default render API
        RenderAPI RZGraphicsContext::s_RenderAPI = RenderAPI::VULKAN;

        void RZGraphicsContext::Create(const WindowProperties& properties, RZWindow* window) {

            switch (s_RenderAPI) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL:    s_Context = new OpenGLContext((GLFWwindow*) window->GetNativeWindow()); break;
#endif
                case Razix::Graphics::RenderAPI::VULKAN:    s_Context = (RZGraphicsContext*) new VKContext(window);                                      break;
                case Razix::Graphics::RenderAPI::METAL:     s_Context = (RZGraphicsContext*) new MTLContext(window);                                     break;
#ifdef RAZIX_RENDER_API_DIRECTX11
                case Razix::Graphics::RenderAPI::DIRECTX11: s_Context = new DX11Context(window);                                    break;
#endif
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: s_Context = nullptr; break;
            }
        }

        void RZGraphicsContext::Release() {
            s_Context->Destroy();
            delete s_Context;
        }

        RZGraphicsContext* RZGraphicsContext::GetContext() {
//            switch (s_RenderAPI) {
//#ifdef RAZIX_RENDER_API_OPENGL
//                case Razix::Graphics::RenderAPI::OPENGL:    return (s_Context); break;
//#endif
//                case Razix::Graphics::RenderAPI::VULKAN:    return (s_Context); break;
//                case Razix::Graphics::RenderAPI::METAL:     return (s_Context); break;
//#ifdef RAZIX_RENDER_API_DIRECTX11
//                case Razix::Graphics::RenderAPI::DIRECTX11: return (s_Context); break;
//#endif
//                case Razix::Graphics::RenderAPI::DIRECTX12:
//                case Razix::Graphics::RenderAPI::GXM:
//                case Razix::Graphics::RenderAPI::GCM:
//                default:                                    return s_Context; break;
//            }
//            return nullptr;
            return s_Context;
        }

        const std::string Graphics::RZGraphicsContext::GetRenderAPIString() {
            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL:    return "OpenGL";            break;
                case Razix::Graphics::RenderAPI::VULKAN:    return "Vulkan";            break;
                case Razix::Graphics::RenderAPI::METAL:     return "Metal";             break;
                case Razix::Graphics::RenderAPI::DIRECTX11: return "DirectX 11";        break;
                case Razix::Graphics::RenderAPI::DIRECTX12: return "DirectX 12";        break;
                case Razix::Graphics::RenderAPI::GXM:       return "SCE GXM (PSVita)";  break;
                case Razix::Graphics::RenderAPI::GCM:       return "SCE GCM (PS3)";     break;
                default:                                    return "None";              break;
            }
        }
    }
}



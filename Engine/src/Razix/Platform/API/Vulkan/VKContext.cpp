// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKContext.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Core/RZApplication.h"
    #include "Razix/Core/RZProfiling.h"
    #include "Razix/Core/RazixVersion.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"

    #define VK_USE_PLATFORM_WIN32_KHR

    #include <glfw/glfw3.h>
    #include <vulkan/vulkan.h>
    #include <vulkan/vulkan_win32.h>

    #define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace Razix {
    namespace Graphics {

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Load the functions dynamically to create the DebugUtilsMessenger
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            else
                return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr)
                func(instance, debugMessenger, pAllocator);
            else
                RAZIX_CORE_ERROR("DestroyDebugUtilsMessengerEXT Function not found");
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        VKContext::VKContext(RZWindow* windowHandle)
            : m_Window(windowHandle)
        {
            RAZIX_CORE_ASSERT(windowHandle, "[Vulkan] Window Handle is NULL!");
        }

        void VKContext::Init()
        {
            // Create the Vulkan instance to interface with the Vulkan library
            createInstance();

            if (RZApplication::Get().getAppType() == AppType::GAME) {
                SetupDeviceAndSC();
            }
        }

        void VKContext::Destroy()
        {
            // Destroy the swapchain
            m_Swapchain->Destroy();
            // Destroy the logical device
            VKDevice::Get().destroy();
            // Destroy the surface
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            // Destroy the debug manager
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugCallbackHandle, nullptr);
            // Destroy the instance at last
            vkDestroyInstance(m_Instance, nullptr);
        }

        void VKContext::Wait()
        {
            vkDeviceWaitIdle(VKDevice::Get().getDevice());
        }

        void VKContext::SetupDeviceAndSC()
        {
            // Create the Logical Device
            VKDevice::Get().init();

            // Create the swapchain (will be auto initialized)
            m_Swapchain = rzstl::CreateRef<VKSwapchain>(m_Window->getWidth(), m_Window->getHeight());

    #ifndef RAZIX_DISTRIBUTION
        #if RZ_PROFILER_OPTICK
            auto device         = VKDevice::Get().getDevice();
            auto physicalDevice = VKDevice::Get().getGPU();
            auto queuefam       = VKDevice::Get().getGraphicsQueue();
            u32  numQueues      = VKDevice::Get().getPhysicalDevice()->getGraphicsQueueFamilyIndex();
            OPTICK_GPU_INIT_VULKAN(&device, &physicalDevice, &queuefam, &numQueues, 1, nullptr);
        #endif    // RZ_PROFILER_OPTICK

    #endif    // RAZIX_DISTRIBUTION
        }

        void VKContext::createInstance()
        {
            // Vulkan Application Info
            VkApplicationInfo appInfo{};
            appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName   = RZApplication::Get().getAppName().c_str();
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);    // TODO: Add this feature later! once we add it to the Application class
            appInfo.pEngineName        = "Razix Engine";
            appInfo.engineVersion      = VK_MAKE_VERSION(RazixVersion.getVersionMajor(), RazixVersion.getVersionMinor(), RazixVersion.getVersionPatch());
            appInfo.apiVersion         = VK_API_VERSION_1_3;

            // Instance Create Info
            VkInstanceCreateInfo instanceCI{};
            instanceCI.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCI.pApplicationInfo = &appInfo;

            // To track if there is any issue with instance creation we supply the pNext with the `VkDebugUtilsMessengerCreateInfoEXT`
            m_DebugCI                 = {};
            m_DebugCI.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            m_DebugCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            m_DebugCI.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            m_DebugCI.pfnUserCallback = debugCallback;

            instanceCI.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &m_DebugCI;

            // Get the Required Instance Layers from the app/engine
            m_RequiredInstanceLayerNames = getRequiredLayers();
            // Get the Required Instance aka Global Extension (also applicable for the device and application)
            m_RequiredInstanceExtensionNames = getRequiredExtensions();

            // Get the Instance Layers and Extensions
            instanceCI.enabledLayerCount       = static_cast<u32>(m_RequiredInstanceLayerNames.size());
            instanceCI.ppEnabledLayerNames     = m_RequiredInstanceLayerNames.data();
            instanceCI.enabledExtensionCount   = static_cast<u32>(m_RequiredInstanceExtensionNames.size());
            instanceCI.ppEnabledExtensionNames = m_RequiredInstanceExtensionNames.data();

            if (VK_CHECK_RESULT(vkCreateInstance(&instanceCI, nullptr, &m_Instance)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create Instance!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Succesfully created Instance!");

            // Create the debug utils
            setupDebugMessenger();

            if (RZApplication::Get().getAppType() == AppType::GAME) {
                CreateSurface((GLFWwindow*) m_Window->GetNativeWindow());
            }
        }

        std::vector<cstr> VKContext::getRequiredLayers()
        {
            std::vector<cstr> layers;
            if (m_EnabledValidationLayer) {
                layers.emplace_back(VK_LAYER_KHRONOS_VALIDATION_NAME);
            }
            return layers;
        }

        std::vector<cstr> VKContext::getRequiredExtensions()
        {
            // First we are sending in the list of desired extensions by GLFW to interface with the WPI
            u32   glfwExtensionsCount = 0;
            cstr* glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
            RAZIX_CORE_TRACE("[Vulkan] GLFW loaded extensions count : {0}", glfwExtensionsCount);

            // This is just for information and Querying purpose
    #ifdef RAZIX_DEBUG
            // Get the total list of supported Extension by Vulkan
            u32 supportedExtensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
            std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

            RAZIX_CORE_TRACE("Available Instance Extensions:\n");
            for (const auto& extension: supportedExtensions) {
                RAZIX_CORE_TRACE("\t {0} \n", extension.extensionName);
            }

            RAZIX_CORE_TRACE("GLFW Requested Extensions are : \n");
            for (u32 i = 0; i < glfwExtensionsCount; i++) {
                RAZIX_CORE_TRACE("\t");
                int j = 0;
                while (*(glfwExtensions[i] + j) != NULL) {
                    std::cout << *(glfwExtensions[i] + j);
                    j++;
                }
                std::cout << std::endl;
            }
    #endif

            // Bundle all the required extensions into a vector and return it
            std::vector<cstr> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
            extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
            extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            // Add any custom extension from the list of supported extensions that you need and are not included by GLFW
            if (m_EnabledValidationLayer)
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            return extensions;
        }

        void VKContext::setupDebugMessenger()
        {
            if (CreateDebugUtilsMessengerEXT(m_Instance, &m_DebugCI, nullptr, &m_DebugCallbackHandle) != VK_SUCCESS)
                RAZIX_CORE_ERROR("[Vulkan] Failed to create debug messenger!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Succesfully created debug messenger!");
        }

        void VKContext::CreateSurface(void* window)
        {
            if (RZApplication::Get().getAppType() == AppType::GAME) {
                if (glfwCreateWindowSurface(m_Instance, (GLFWwindow*) window, nullptr, &m_Surface))
                    RAZIX_CORE_ERROR("[Vulkan] Failed to create surface!");
                else
                    RAZIX_CORE_TRACE("[Vulkan] Succesfully created surface!");
            } else {
                // if the app type is editor create a custom surface based on the OS
    #ifdef RAZIX_PLATFORM_WINDOWS
                //HWND*                       hwndPtr = (HWND*) window;
                //HWND                        hwnd    = *hwndPtr;
                //VkWin32SurfaceCreateInfoKHR createInfo{};
                //createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                //createInfo.hwnd      = hwnd;
                //createInfo.hinstance = GetModuleHandle(nullptr);

                //if (vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface))
                //    RAZIX_CORE_ERROR("[Vulkan] Failed to create surface! for native window");
                //else
                //    RAZIX_CORE_TRACE("[Vulkan] Successfully created surface for native window!");

                m_Surface = *(VkSurfaceKHR*) window;
    #endif
            }
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL VKContext::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
        {
    #ifndef RAZIX_DISTRIBUTION
            // Select prefix depending on flags passed to the callback
            // Note that multiple flags may be set for a single validation message
            // Error that may result in undefined behavior
            // TODO: Add option to choose minimum severity level and use <=> to select levels
            // TODO: Formate the message id and stuff for colors etc

            //return VK_FALSE;

            //if (!message_severity)
            //    return VK_FALSE;

            for (sz i = 0; i < callback_data->objectCount; i++) {
                if (callback_data->pObjects[i].pObjectName)
                    RAZIX_CORE_ERROR("[VULKAN] OBJECT HANDLE NAME : {0}", callback_data->pObjects[i].pObjectName);
            }

            if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
                std::cout << "\033[1;31m ***************************************************************** \033[0m" << std::endl;
                std::cout << "\033[1;32m[VULKAN] \033[1;31m - Validation ERROR : \033[0m \nmessage ID : " << callback_data->messageIdNumber << "\nID Name : " << callback_data->pMessageIdName << "\nMessage : " << callback_data->pMessage << std::endl;
                std::cout << "\033[1;31m ***************************************************************** \033[0m" << std::endl;
            };
            // Warnings may hint at unexpected / non-spec API usage
            if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                std::cout << "\033[1;33m ***************************************************************** \033[0m" << std::endl;
                std::cout << "\033[1;32m[VULKAN] \033[1;33m - Validation WARNING : \033[0m \nmessage ID : " << callback_data->messageIdNumber << "\nID Name : " << callback_data->pMessageIdName << "\nMessage : " << callback_data->pMessage << std::endl;
                std::cout << "\033[1;33m ***************************************************************** \033[0m" << std::endl;
            };
            // Informal messages that may become handy during debugging
            if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
                std::cout << "\033[1;36m ***************************************************************** \033[0m" << std::endl;
                std::cout << "\033[1;32m[VULKAN] \033[1;36m - Validation INFO : \033[0m \nmessage ID : " << callback_data->messageIdNumber << "\nID Name : " << callback_data->pMessageIdName << "\nMessage : " << callback_data->pMessage << std::endl;
                std::cout << "\033[1;36m ***************************************************************** \033[0m" << std::endl;
            }
            // Diagnostic info from the Vulkan loader and layers
            // Usually not helpful in terms of API usage, but may help to debug layer and loader problems
            if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
                std::cout << "\033[1;35m*****************************************************************" << std::endl;
                std::cout << "\033[1;32m[VULKAN] \033[1;35m - DEBUG : \033[0m \nmessage ID : " << callback_data->messageIdNumber << "\nID Name : " << callback_data->pMessageIdName << "\nMessage : " << callback_data->pMessage << std::endl;
                std::cout << "\033[1;35m*****************************************************************" << std::endl;
            }
    #endif
            return VK_FALSE;
        }
    }    // namespace Graphics
}    // namespace Razix
#endif
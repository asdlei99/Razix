-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'

-- Shaders a separate project to build as cache
group "Engine/content"
    project "Shaders"
        kind "Utility"

        files
        {
            -- Shader files
            -- GLSL
            "content/Shaders/GLSL/**.glsl",
            "content/Shaders/GLSL/**.vert",
            "content/Shaders/GLSL/**.geom",
            "content/Shaders/GLSL/**.frag",
            "content/Shaders/GLSL/**.comp",
            -- HLSL
            "content/Shaders/HLSL/**.hlsl",
            -- PSSL
            "content/Shaders/PSSL/**.pssl",
            "content/Shaders/PSSL/**.h",
            "content/Shaders/PSSL/**.hs",
            -- Cg
            "content/Shaders/CG/**.cg",
            -- Razix Shader Files
            "content/Shaders/Razix/**.rzsf"
        }
    filter "system:windows"
        -- TODO Add as rules, every shader file type will have it's own rule
        -- Don't build the shaders, they are compiled by the engine once and cached
       filter { "files:**.glsl or **.hlsl or **.pssl or **.cg or **.rzsf"}
            flags { "ExcludeFromBuild" }

        -- Build GLSL files based on their extension
        filter {"files:**.vert or **.frag or **.geom"}
            removeflags "ExcludeFromBuild"
            buildmessage 'Compiling glsl shader : %{file.name}'
            buildcommands 'glslc.exe -I "%{wks.location}/../Engine/content/Shaders/GLSL" "%{file.directory}/%{file.name}" -o "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.name }.spv" '
            buildoutputs "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.name }.spv"
group""

-- Frame Graph resources data for editing in VS
group "Engine/content"
    project "FrameGraphs"
        kind "Utility"

        files
        {
            -- Graph files
            "content/FrameGraphs/Graphs/**.json",
            -- Pass files
            "content/FrameGraphs/Passes/**.json",
        }

        filter { "files:**.json"}
            flags { "ExcludeFromBuild" }

group""


group "Engine"
-- Razix project
project "Razix"
    kind "SharedLib"
    language "C++"
        -- Debugging directory = where the main premake5.lua is located
    debugdir "%{wks.location}../"

    pchheader "src/rzxpch.h"
    pchsource "src/rzxpch.cpp"

    -- Razix Engine defines (Global)
    defines
    {
        --Razix
        "RAZIX_ENGINE",
        "RAZIX_BUILD_DLL",
        "RAZIX_ROOT_DIR="  .. root_dir,
        "RAZIX_BUILD_CONFIG=" .. outputdir,
        -- Renderer
        "RAZIX_RENDERER_RAZIX",
        "RAZIX_RENDERER_FALCOR",
        "RAZIX_RAY_TRACE_RENDERER_RAZIX",
        "RAZIX_RAY_TRACE_RENDERER_OPTIX",
        "RAZIX_RAY_TRACE_RENDERER_EMBREE",
        -- vendor
        "OPTICK_MSVC"
    }

    -- Razix Engine source files (Global)
    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        "src/**.inl",
        "src/**.tpp"
        -- vendor
        --"vendor/tracy/TracyClient.cpp",
    }

    -- Lazily add the platform files based on OS config
    removefiles
    {
        "src/Razix/Platform/**"
    }

    -- For MacOS
    externalincludedirs
    {
        -- Engine
        "./",
        "../",
        "internal/",
        "src/",
        "src/Razix",
        -- Vendor
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.SPIRVReflect}",
        "%{IncludeDir.SPIRVCross}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.tracy}",
        "%{IncludeDir.optick}",
        "%{IncludeDir.Jolt}",
        "%{IncludeDir.json}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- API related
        -- Vulkan
        "%{VulkanSDK}",
        -- Internal libraries
        "%{InternalIncludeDir.RazixMemory}",
        "%{InternalIncludeDir.RZSTL}",
        "%{InternalIncludeDir.EASTL}",
        "%{InternalIncludeDir.EABase}"
    }

    -- Razix engine external linkage libraries (Global)
    links
    {
        "glfw",
        "imgui",
        "spdlog", -- Being linked staically by RazixMemory (Only include this in debug and release build exempt this in Distribution build)
        "SPIRVReflect",
        "SPIRVCross",
        "meshoptimizer",
        "OpenFBX", 
        "lua",
        "optick",
        "tracy",
        "Jolt",
        -- Shaders
        "Shaders",
        -- Razix Internal Libraries 
        -- 1. Razix Memory
        "RazixMemory",
        "RZSTL"
    }

    -- Disable PCH for vendors
    filter 'files:vendor/**.cpp'
        flags  { 'NoPCH' }
    filter 'files:vendor/**.c'
        flags  { 'NoPCH' }

     -- Disable warning for vendor
    filter { "files:vendor/**"}
        warnings "Off"

    -- Razix Project settings for Windows
    filter "system:windows"
        cppdialect "C++20"
        staticruntime "off"
        systemversion "latest"
        disablewarnings { 4307, 4267, 4275, 4715, 4554 } -- Disabling the 4275 cause this will propagate into everything ig, also 4715 = not returinign values from all control paths is usually done deliberately hence fuck this warning
        characterset ("MBCS")
        editandcontinue "Off"

         -- Enable AVX, AVX2, Bit manipulation Instruction set (-mbmi)
         -- because GCC uses fused-multiply-add (fma) instruction by default, if it is available. Clang, on the contrary, doesn't use them by default, even if it is available, so we enable it explicityly
        -- Only works with GCC and Clang
        --buildoptions { "-mavx", "-mavx2", "-mbmi", "-march=haswell"}--, "-mavx512f -mavx512dq -mavx512bw -mavx512vbmi -mavx512vbmi2 -mavx512vl"}
        --buildoptions {"/-fsanitize=address"}

        pchheader "rzxpch.h"
        pchsource "src/rzxpch.cpp"

        -- Build options for Windows / Visual Studio (MSVC)
        -- https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170 
        buildoptions
        {
            "/MP", "/bigobj", "/Zi"
        }

        linkoptions
        {
            --"/NODEFAULTLIB:libcpmt.lib" ,"/NODEFAULTLIB:msvcprt.lib", "/NODEFAULTLIB:libcpmtd.lib", "/NODEFAULTLIB:msvcprtd.lib"
        }

        -- Windows specific defines
        defines
        {
            -- Engine
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_OPENGL",
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX11",
            "RAZIX_RENDER_API_DIRECTX12",
            -- Windows / Vidual Studio
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
            "TRACY_ENABLE",
            -- build options
            "_DISABLE_VECTOR_ANNOTATION",
            "_DISABLE_STRING_ANNOTATION"
        }

        -- Windows specific source files for compilation
        files
        {
            -- platform sepecific implementatioon
            "src/Razix/Platform/Windows/*.h",
            "src/Razix/Platform/Windows/*.cpp",

            "src/Razix/Platform/GLFW/*.h",
            "src/Razix/Platform/GLFW/*.cpp",

            -- Platform supported Graphics API implementatioon
            "src/Razix/Platform/API/OpenGL/*.h",
            "src/Razix/Platform/API/OpenGL/*.cpp",

            "src/Razix/Platform/API/Vulkan/*.h",
            "src/Razix/Platform/API/Vulkan/*.cpp",

            "src/Razix/Platform/API/DirectX11/*.h",
            "src/Razix/Platform/API/DirectX11/*.cpp",

            "src/Razix/Platform/API/DirectX12/*.h",
            "src/Razix/Platform/API/DirectX12/*.cpp",

            -- Vendor source files
            "vendor/glad/src/glad.c"
        }

        -- Windows specific incldue directories
        includedirs
        {
             VulkanSDK .. "/include"
        }

        -- Windows specific library directories
        libdirs
        {
            VulkanSDK .. "/Lib"
        }

        -- Windows specific linkage libraries (DirectX inlcude and library paths are implicityly added by Visual Studio, hence we need not add anything explicityly)
        links
        {
            "Dbghelp",
            -- Redner API
            "vulkan-1",
            "d3d11",
            "d3d12",
            "D3DCompiler"
        }

    -- Config settings for Razix Engine project
    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION", "NDEBUG" }
        symbols "On"
        optimize "Full"
        runtime "Release"
group""
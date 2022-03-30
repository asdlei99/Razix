project "SPIRVReflect"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "common/output_stream.h",
        "common/output_stream.cpp",
        "include/spirv/unified1/spirv.h",
        "spirv_reflect.c",
        "spirv_reflect.h"
    }

    includedirs
    {
        "include"
    }

    filter "system:linux or system:macosx"
        pic "On"
        systemversion "latest"
        staticruntime "off"

    filter "system:windows"
        systemversion "latest"
        staticruntime "off"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        symbols "Off"
        optimize "Full"

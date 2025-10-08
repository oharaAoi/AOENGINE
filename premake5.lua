-- ===============================================================
-- AOENGINE Premake5 Build Script
-- ===============================================================
workspace "AOENGINE"
    architecture "x64"
    configurations { "Debug", "Release" }
    location "Generated"
    startproject "Game"

    filter "system:windows"
        systemversion "latest"
        buildoptions { "/utf-8" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Full"
        runtime "Release"

project "Game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    location "Generated/Game"
    targetdir "%{wks.location}/../Project/bin/%{cfg.buildcfg}/%{prj.name}"
    objdir    "%{wks.location}/../Project/bin-int/%{cfg.buildcfg}/%{prj.name}"

    files {
        "Project/Game/**.h",
        "Project/Game/**.cpp",
        "Project/Engine/**.h",
        "Project/Engine/**.cpp"
    }

    includedirs {
        "Project",
        "Project/Engine",
        "Project/Externals",
        "Project/Externals/assimp/include",
        "Project/Externals/DirectXTex",
        "Project/Externals/ImGui",
        "Project/Externals/nlohmann"
    }

    libdirs {
        "Project/Externals/assimp/lib"
    }

    links {
        "d3d12",
        "dxgi",
        "dxguid",
        "assimp-vc143-mt",
        "DirectXTex"
    }

    debugdir "Project/Game"
    -- DirectXTexを参照プロジェクトとして追加
    dependson { "DirectXTex" }

    postbuildcommands {
        '{COPY} "%{wks.location}/../Project/Externals/assimp/bin/*.dll" "%{cfg.targetdir}"'
    }

-- ===============================================================
-- 外部プロジェクト登録
-- ===============================================================

externalproject "DirectXTex"
    location "Project/Externals/DirectXTex"
    kind "StaticLib"
    language "C++"
    filename "DirectXTex_Desktop_2022_Win10"
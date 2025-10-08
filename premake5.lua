workspace "AOENGINE"
    architecture "x64"
    configurations { "Debug", "Release" }
    location "Project"
    startproject "Game"

-- ===============================================================
-- 共通設定
-- ===============================================================
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Full"
        runtime "Release"

-- ===============================================================
-- [2] Game プロジェクト（実行ファイル）
-- ===============================================================

project "Project"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    location "Project"
    targetdir "bin/%{cfg.buildcfg}"
    objdir    "bin-int/%{cfg.buildcfg}/Game"

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

    -- Engine と外部リンク
    links {
        "d3d12",
        "dxgi",
        "dxguid",
        "assimp-vc143-mt",
        "DirectXTex"
    }

    -- 実行時ディレクトリを指定
    debugdir "Project/Game"

    -- DLL 自動コピー
    postbuildcommands {
        '{COPY} "%{wks.location}/../Project/Externals/assimp/bin/*.dll" "%{cfg.targetdir}"'
    }
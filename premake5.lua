-- ===============================================================
-- DXC (dxcompiler.dll, dxil.dll) コピー関数
-- ===============================================================
function Add_DXC_DLL_CopyCommands()
    local sdkRoot = "C:/Program Files (x86)/Windows Kits/10/bin/"
    local sdkVer = "10.0.26100.0"
    local base = sdkRoot .. sdkVer .. "/x64/"
    local dxcompilerPath = base .. "dxcompiler.dll"
    local dxilPath       = base .. "dxil.dll"

    return {
        'echo 🔧 Copying DirectX Compiler DLLs...',
        string.format('if not exist "$(TargetDir)" mkdir "$(TargetDir)"'),
        string.format('if exist "%s" xcopy /Y /I "%s" "$(TargetDir)"', dxcompilerPath, dxcompilerPath),
        string.format('if exist "%s" xcopy /Y /I "%s" "$(TargetDir)"', dxilPath, dxilPath),
        'echo ✅ DXC copy done.'
    }
end


-- ===============================================================
-- AOENGINE Premake5 Build Script
-- ===============================================================
workspace "AOENGINE"
    architecture "x64"
    configurations { "Debug", "Release" }
    location "Project"
    startproject "Game"

    filter "system:windows"
        systemversion "latest"
        buildoptions { "/utf-8" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        staticruntime "on"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Full"
        staticruntime "on"
        runtime "Release"

project "Game"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"

    -- 🔹 各 .vcxproj の出力先（Project内）
    location "Project"

    -- 🔹 exe / pdb / dll などの出力先
    targetdir "%{wks.location}/../Generated/Outputs/%{cfg.buildcfg}/"
    objdir    "%{wks.location}/../Generated/Outputs/Intermediate/%{cfg.buildcfg}/%{prj.name}"
    debugdir  ("%{wks.location}/..")

    -- 🔹 ソース
    files {
        "Project/main.cpp",
        "Project/Enviroment.h",
        "Project/Game/**.h", "Project/Game/**.cpp",
        "Project/Engine/**.h", "Project/Engine/**.cpp",
        "Project/Externals/ImGui/**.h", "Project/Externals/ImGui/**.cpp"
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

    filter "configurations:Debug"
        links { "d3d12", "dxgi", "dxguid", "DirectXTex", "assimp-vc143-mtd" }
        libdirs { "%{wks.location}/../Project/Externals/assimp/bin/Debug" }

        postbuildcommands {
            'if not exist "$(TargetDir)" mkdir "$(TargetDir)"',
            'copy /Y "%{wks.location}/../Project/Externals/assimp/bin/Debug/assimp-vc143-mtd.dll" "$(TargetDir)"',
            'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"',
            'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"'
        }

    filter "configurations:Release"
        links { "d3d12", "dxgi", "dxguid", "DirectXTex", "assimp-vc143-mt" }
        libdirs { "%{wks.location}/../Project/Externals/assimp/bin/Release" }

        postbuildcommands {
            'if not exist "$(TargetDir)" mkdir "$(TargetDir)"',
            'copy /Y "%{wks.location}/../Project/Externals/assimp/bin/Release/assimp-vc143-mt.dll" "$(TargetDir)"',
            'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"',
            'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"'
        }

    filter {}
    dependson { "DirectXTex", "ImGui" }

-- ===============================================================
-- 外部プロジェクト
-- ===============================================================
externalproject "DirectXTex"
    location "Project/Externals/DirectXTex"
    kind "StaticLib"
    language "C++"
    filename "DirectXTex_Desktop_2022_Win10"

externalproject "ImGui"
    location "Project/Externals/ImGui"
    kind "StaticLib"
    language "C++"
    filename "ImGui"
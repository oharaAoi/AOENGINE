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
        'echo Copying DirectX Compiler DLLs...',
        string.format('if not exist "$(TargetDir)" mkdir "$(TargetDir)"'),
        string.format('if exist "%s" xcopy /Y /I "%s" "$(TargetDir)"', dxcompilerPath, dxcompilerPath),
        string.format('if exist "%s" xcopy /Y /I "%s" "$(TargetDir)"', dxilPath, dxilPath),
        'echo DXC copy done.'
    }
end


-- ===============================================================
-- AOENGINE Premake5 Build Script
-- ===============================================================
workspace "AOENGINE" -- ソリューションの名前
    architecture "x64" -- 構成プラットフォーム
    configurations { "Debug", "Release" } -- ビルド構成
    location "Project" -- 配置するフォルダ
    startproject "AOENGINE" -- 最初に起動するプロジェクト
    flags {"MultiProcessorCompile"} -- 複数プロサッサによるコンパイル
    warnings "Extra" -- 警告レベル

    filter "system:windows" -- windowsのときに起動する
        systemversion "latest" -- 最新のwindowsSDKを使用してビルドする
        buildoptions { "/utf-8" } -- ソースコードの文字コード

    filter "configurations:Debug" -- DEBUG時の設定
        defines { "DEBUG" } -- #define DEBUGを有効化
        symbols "On" -- .pdbファイルを生成する
        staticruntime "on" -- 静的リンクにする
        runtime "Debug" -- ランタイムをMDd or MTdにする

    filter "configurations:Release"
        defines { "NDEBUG" } -- #define NDEBUGを有効化
        optimize "Full" -- コンパイラ最適化を最大にする(o2)
        staticruntime "on" -- 静的ランタイムリンク
        runtime "Release" -- MT or MD

-- ===============================================================
-- Game Build Script
-- ===============================================================
project "AOENGINE" -- .vcxprojを定義する
    kind "WindowedApp" -- windowのアプリにする
    language "C++" -- 言語設定
    cppdialect "C++20" -- C++の規格設定

    -- 各 .vcxproj の出力先（Project内）
    location "Project"

    -- exe / pdb / dll などの出力先
    targetdir "%{wks.location}/../Generated/Outputs/%{cfg.buildcfg}/"
    objdir    "%{wks.location}/../Generated/Outputs/Intermediate/%{cfg.buildcfg}/%{prj.name}"
    debugdir  ("%{wks.location}/..")

    -- ソース登録
    files {
        "Project/main.cpp",
        "Project/Enviroment.h",
        "Project/Game/**.h", "Project/Game/**.cpp",
        "Project/Engine/**.h", "Project/Engine/**.cpp",
        "Project/Externals/ImGui/**.h", "Project/Externals/ImGui/**.cpp"
    }

    -- 追加のインクルード
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
        -- 追加の依存ファイル
        links { "d3d12", "dxgi", "dxguid", "DirectXTex", "assimp-vc143-mtd" }
        -- 追加のライブラリ
        libdirs { "%{wks.location}/../Project/Externals/assimp/lib/Debug" }

        -- ビルド後イベント
        postbuildcommands {
            'if not exist "$(TargetDir)" mkdir "$(TargetDir)"',
            'copy /Y "%{wks.location}\\Externals\\assimp\\lib\\%{cfg.buildcfg}\\assimp-vc143-mtd.dll" "$(TargetDir)"',
            'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"',
            'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"'
        }

    filter "configurations:Release"
        -- 追加の依存ファイル
        links { "d3d12", "dxgi", "dxguid", "DirectXTex", "assimp-vc143-mt" }
        -- 追加のライブラリ
        libdirs { "%{wks.location}/../Project/Externals/assimp/lib/Release" }

        postbuildcommands {
            'if not exist "$(TargetDir)" mkdir "$(TargetDir)"',
            'copy /Y "%{wks.location}\\Externals\\assimp\\lib\\%{cfg.buildcfg}\\assimp-vc143-mt.dll" "$(TargetDir)"',
            'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"',
            'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"'
        }

    -- フィルタ解除(Dbug/Release共通の設定にする)
    filter {}
    -- プロジェクト依存関係を設定
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
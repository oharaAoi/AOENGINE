#pragma once
#include <Windows.h>
#include <cstdint>
#ifdef _DEBUG
#include "Engine/System/Manager/ImGuiManager.h"
#endif

/// <summary>
/// ウィンドウズアプリケーション
/// </summary>
class WinApp{
public: // 静的メンバ変数
	// ウィンドウサイズ
	static uint32_t sWindowWidth;	// 横幅
	static uint32_t sWindowHeight;	// 縦幅
	
public: // 静的メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static WinApp* GetInstance();

	/// <summary>
	/// ウィンドウプロージャー
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ番号</param>
	/// <param name="wparam">メッセージ情報1</param>
	/// <param name="lparam">メッセージ情報2</param>
	/// <returns>成否</returns>
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: // メンバ関数
	WinApp() = default;
	~WinApp();
	WinApp(const WinApp& obj) = delete;
	const WinApp& operator=(const WinApp&) = delete;

	void CreateGameWindow(uint32_t _backBufferWidth, uint32_t _backBufferHeight, const char* _windowTitle);

	void SetFullScreen(bool fullscreen);

	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns>終了かどうか</returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd() const { return hwnd_; }

	/// <summary>
	/// windowクラスの取得
	/// </summary>
	/// <returns></returns>
	WNDCLASS GetWNDCLASS() const { return wc; }

	void Finalize();

private: // メンバ変数
	// window関連
	HWND hwnd_ = nullptr; // ウィンドウハンドル
	WNDCLASS wc{};
	RECT windowRect_;
	UINT windowStyle_ = WS_OVERLAPPEDWINDOW;

	bool isFullscreen_;
};


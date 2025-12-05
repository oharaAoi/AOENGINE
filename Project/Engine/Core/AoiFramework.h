#pragma once
#include <cstdint>
#include "Engine/Utilities/Logger.h"

namespace AOENGINE {

/// <summary>
/// ゲーム全体
/// </summary>
class AoiFramework {
public:	// コンストラクタ

	AoiFramework(uint32_t _kWindowWidth, uint32_t _kWindowHeight, const char* _windowTitle);
	virtual ~AoiFramework() = default;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Init();

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// Frame開始処理
	/// </summary>
	virtual void BeginFrame() = 0;

	/// <summary>
	/// Frame終了処理
	/// </summary>
	virtual void EndFrame() = 0;

	/// <summary>
	/// 実行処理
	/// </summary>
	void Run();

	/// <summary>
	/// 終了判定
	/// </summary>
	/// <returns>: 終了</returns>
	virtual bool isEndRequest() { return endRequest_; }

private:

	uint32_t windowWidth_ = 1280;
	uint32_t windowHeight_ = 720;

	const char* windowTitle_;

	bool endRequest_;

	Logger logger_;

};

}
#pragma once
#include "Engine/Utilities/Logger.h"

/// <summary>
/// ゲーム全体
/// </summary>
class AoiFramework {
public:	// コンストラクタ

	AoiFramework() {};
	virtual ~AoiFramework() {};

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
	virtual void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

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

	bool endRequest_;

	Logger logger_;

};


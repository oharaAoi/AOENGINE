#pragma once
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class FollowCamera;

/// <summary>
/// カメラアニメーションの基底クラス
/// </summary>
class ICameraAnimation :
	public AttributeGui {
public: // コンストラクタ

	ICameraAnimation() = default;
	virtual ~ICameraAnimation() = default;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 実行処理を呼ぶ
	/// </summary>
	/// <param name="_isRevers">: Animationを反転させるか</param>
	virtual void CallExecute(bool _isRevers) = 0;

	/// <summary>
	/// 編集処理
	/// </summary>
	virtual void Debug_Gui() override = 0;

public:

	void SetFollowCamera(FollowCamera* _followCamera) { pFollowCamera_ = _followCamera; }

	bool GetIsFinish() const { return isFinish_; }

protected:

	FollowCamera* pFollowCamera_ = nullptr;

	bool isFinish_ = false;

};


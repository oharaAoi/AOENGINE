#pragma once
#include <memory>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class FadePanel :
	public AttributeGui {
public:

	FadePanel() = default;
	~FadePanel() = default;

	void Init();
	void Update();
	void Draw() const;

	void Debug_Gui() override;

public:

	/// <summary>
	/// ブラックアウトをセットする
	/// </summary>
	void SetBlackOut(float timeLimit = 1.0f);

	/// <summary>
	/// ブラックアウトから開けさせる
	/// </summary>
	void SetBlackOutOpen(float timeLimit = 1.0f);

	/// <summary>
	/// 終了状態かを取得する
	/// </summary>
	/// <returns></returns>
	const bool GetIsFinished() const { return isFinished_; }

	const bool GetDoNoting() const { return doNothing_; }

private:

	std::unique_ptr<Sprite> panel_;

	float panelAlpha_ = 0.0f;

	float time_ = 0.0f;
	float timeLimit_ = 1.0f;

	bool isBlackOut_;
	bool doNothing_;	// 何もしないフラグ
	bool isFinished_;	// 終了した

};


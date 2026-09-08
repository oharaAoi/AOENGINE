#pragma once

#include <vector>

// engine
#include <Engine/Utilities/Timer.h>

#include <Engine/Module/Components/2d/Sprite.h>

#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Easing.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

enum class RetryItem {
	Pause, // 待機中
	Retry, // リトライ
	Title  // タイトルへ
};

struct RetryParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	float fallTime = 1.f;
	float diffLength = 500.f;
	int easeKind = 0;

	RetryParameter() : CustomParameterSet("RetryParameter") {
		SetGroupName("UI");
		SetName("RetryParameter");

		AddParameter("落ちる時間", fallTime);
		AddParameter("落ちる距離(共通)", diffLength);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("fallTime", fallTime)
			.Add("diffLength", diffLength)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "fallTime", fallTime);
		Convert::fromJson(jsonData, "diffLength", diffLength);
	}
};

/// <summary>
/// リトライを管理しているクラス
/// </summary>
class RetryUI {
public: // constructor

	RetryUI() = default;
	~RetryUI() = default;

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// Playerが存命していれば
	/// </summary>
	/// <param name="isPlayerAlive"></param>
	RetryItem Update(bool isPlayerAlive);

private: // private variable

	/// <summary>
	/// 次のアクションを選択する
	/// </summary>
	void SelectItem();

	/// <summary>
	/// 次の項目の決定
	/// </summary>
	/// <returns>決定ボタンが押されたかどうか</returns>
	bool DecisionItem();

	/// <summary>
	/// 現在選択中の項目を返す
	/// </summary>
	/// <returns></returns>
	RetryItem CurrentSelect();

	bool StartEffect();

	void SetFall(const Math::Vector2& startPos, const Math::Vector2& endPos, int index);

	/// <summary>
	/// Spriteが落ちる演出
	/// </summary>
	/// <param name="sprite"></param>
	void SpriteFall(AOENGINE::Sprite* sprite, int index);

private: // private variable

	int selectIndex_ = 0;

	AOENGINE::Timer coolTimer_;
	AOENGINE::Timer fallTimer_;

	RetryParameter retryParametor_;

	bool isFall_ = false;

	std::vector<AOENGINE::VectorTween<Math::Vector2>> animationTween_;

};


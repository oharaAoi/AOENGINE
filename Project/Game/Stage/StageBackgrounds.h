#pragma once

// c++
#include <queue>

// engine
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include <Engine/Module/Components/GameObject/SceneObject.h>

// game
#include "Game/Stage/StageBlockField.h"
#include "Game/Stage/StageSegment.h"

/// <summary>
/// 背景スクロールで使用するパラメータ
/// </summary>
struct BackgroundParameter : 
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	BackgroundParameter()
		: CustomParameterSet("BackgroundParameter") {
		AddParameter("背景スクロールをする高さ", scrollHeight, 1.0f, 0.0f, 100.0f);

		SetGroupName("Background");
		SetName("backgroundParameter");
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("scrollHeight", scrollHeight)
			.Build();
	}

	void FromJson(const json& data) override {
		Convert::fromJson(data, "scrollHeight", scrollHeight);
	}

	int scrollHeight; // 背景スクロールをする高さ
};

/// <summary>
/// ステージの背景を所有しているクラス
/// </summary>
class StageBackgrounds {
public: // constructor

	StageBackgrounds() = default;
	~StageBackgrounds() = default;

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(StageBlockField* field, StageSegment* segment);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(StageBlockField* field, StageSegment* segment, const Math::Vector3& playerPos);

private: // private method

	/// <summary>
	/// 背景のループ。プレイヤーが次の背景の高さまで登ったら1枚足し、一番古い1枚を捨てる
	/// </summary>
	void BackgroundLoop(const Math::Vector3& playerPos);

	/// <summary>
	/// 段のループ。プレイヤーが次の段の高さまで登ったら1段積み、画面外に出た段を捨てる
	/// </summary>
	void SegmentLoop(StageBlockField* field, StageSegment* segment, const Math::Vector3& playerPos);

	/// <summary>
	/// 背景を1枚生成する
	/// </summary>
	void CreateBackground();

	/// <summary>
	/// 段を1つ生成する
	/// </summary>
	void CreateSegment(StageBlockField* field, StageSegment* segment);

private: // private variables

	std::queue<AOENGINE::ObjectHandle> objHandles_;

	BackgroundParameter parameter_;

	/// 最後に生成した背景の番号
	int backgroundIndex_ = -1;
	/// 最後に生成した段の番号
	int segmentIndex_ = -1;

};


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
	/// 背景ループ
	/// </summary>
	void BackgroundLoop(StageBlockField* field, StageSegment* segment, const Math::Vector3& playerPos);

	/// <summary>
	/// 背景生成
	/// </summary>
	void CreateBackground();

private: // private variables

	std::queue<AOENGINE::ObjectHandle> objHandles_;

	BackgroundParameter parameter_;

	int currentIndex_ = 1;

};


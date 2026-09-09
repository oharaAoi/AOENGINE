#pragma once

// engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include <Engine/Module/Components/2d/Sprite.h>

struct SceneTransitionParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	int transitionTime = 1.f;					// 爆破回数

	SceneTransitionParameter() : CustomParameterSet("SceneTransitionParameter") {
		SetGroupName("SceneTransition");
		SetName("SceneTransitionParameter");

		AddParameter("遷移時間", transitionTime);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("transitionTime", transitionTime)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "transitionTime", transitionTime);
	}
};

/// <summary>
/// シーン遷移を行うクラス
/// </summary>
class SceneTransition {
public:

	SceneTransition() = default;
	~SceneTransition() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// フェードイン
	/// </summary>
	void FadeIn();

	/// <summary>
	/// フェードアウト
	/// </summary>
	void FadeOut();

public: // accessor

	bool IsFinish() const { return spriteAlphaTween_.GetIsFinish(); };

private: 

	AOENGINE::VectorTween<float> spriteAlphaTween_;

	SceneTransitionParameter parameter_;

	AOENGINE::Sprite* fade_;

};


#pragma once

// engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include <Engine/Module/Components/2d/Sprite.h>

struct SceneTransitionParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	float transitionTime = 0.4f;

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
	void Release();

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
	void SetCovered();
	void SetVisible();

public: // accessor

	bool IsFinish() const { return spriteAlphaTween_.GetIsFinish(); };
	bool IsReady() const;

private: 
	AOENGINE::Sprite* GetFade() const;

	AOENGINE::VectorTween<float> spriteAlphaTween_;

	SceneTransitionParameter parameter_;

	// SceneWorld再構築後に破棄済みSpriteへ触れないよう、生ポインタは保持しない。
	AOENGINE::ObjectHandle fadeHandle_{};

};


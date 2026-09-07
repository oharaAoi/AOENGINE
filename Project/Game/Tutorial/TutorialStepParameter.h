#pragma once

#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

/// <summary>
/// チュートリアルの進み方の調整値。見た目ではなく、ページを送る間合いを持つ
/// </summary>
struct TutorialStepParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// 打ち上げをボスに当ててから、次のページへ送るまでの待ち時間。
	// チェックが出きるのを見せてから送りたいので、チェックの出現時間より長めにしておく
	float launchClearWaitTime = 1.5f;

	TutorialStepParameter() : CustomParameterSet("TutorialStep") {
		SetGroupName("Tutorial");
		SetName("tutorialStep");

		AddParameter("Launch Clear Wait Time", launchClearWaitTime, 0.05f, 0.0f, 30.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("launchClearWaitTime", launchClearWaitTime)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "launchClearWaitTime", launchClearWaitTime);
	}
};

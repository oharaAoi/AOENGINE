#pragma once

#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

/// <summary>
/// タイトルの背景に置く地球の調整値。
/// ボスと同じで、ワールド座標ではなく画面上の位置で指定する
/// </summary>
struct TitleEarthParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// 画面上のどこに置くか。1920x1009基準のピクセル
	Math::Vector2 screenPos{ 916.0f, 695.0f };

	// 置く奥行き。隕石(z=0)より奥にしないと手前に出てしまう
	float worldZ = 25.456f;

	// 見た目の大きさ。モデルが大きいので、かなり小さくして使う
	float scale = 0.01956f;

	TitleEarthParameter() : CustomParameterSet("TitleEarth") {
		SetGroupName("Title");
		SetName("titleEarth");

		AddParameter("Screen Pos(px)", screenPos, 1.0f);
		AddParameter("World Z", worldZ, 0.1f);
		AddParameter("Scale", scale, 0.001f, 0.0001f, 10.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("screenPos", screenPos)
			.Add("worldZ", worldZ)
			.Add("scale", scale)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "screenPos", screenPos);
		Convert::fromJson(jsonData, "worldZ", worldZ);
		Convert::fromJson(jsonData, "scale", scale);
	}
};

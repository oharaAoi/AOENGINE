#pragma once

#include <cstdint>

#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

/// <summary>
/// タイトルの背景に流す隕石の調整値
/// </summary>
struct TitleMeteoParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// 同時に流す数。作る数なので、変えた分はシーンを入り直すと反映される
	int32_t count = 12;

	// 湧く帯の中心と広がり。広がりは中心からの±で見る
	Math::Vector3 spawnCenter{ -14.0f, 10.0f, 0.0f };
	Math::Vector3 spawnSize{ 10.0f, 6.0f, 0.0f };

	// 進む向き
	float fallAngle = -45.0f;

	// 落ちる速さ
	float minSpeed = 7.0f;
	float maxSpeed = 12.0f;

	// この距離まで進んだら、また帯へ戻して落とし直す
	float travelDistance = 45.0f;

	// 火玉の大きさ
	float scale = 1.1f;

	TitleMeteoParameter() : CustomParameterSet("TitleMeteo") {
		SetGroupName("Title");
		SetName("titleMeteo");

		AddParameter("Count", count, 1.0f, 1.0f, 128.0f);
		AddSeparatorText("Spawn");
		AddParameter("Spawn Center", spawnCenter, 0.1f);
		AddParameter("Spawn Size", spawnSize, 0.1f);
		AddSeparatorText("Move");
		AddParameter("Fall Angle(deg)", fallAngle, 1.0f, -180.0f, 180.0f);
		AddParameter("Min Speed", minSpeed, 0.1f, 0.0f, 100.0f);
		AddParameter("Max Speed", maxSpeed, 0.1f, 0.0f, 100.0f);
		AddParameter("Travel Distance", travelDistance, 0.5f, 1.0f, 1000.0f);
		AddParameter("Scale", scale, 0.05f, 0.05f, 20.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("count", count)
			.Add("spawnCenter", spawnCenter)
			.Add("spawnSize", spawnSize)
			.Add("fallAngle", fallAngle)
			.Add("minSpeed", minSpeed)
			.Add("maxSpeed", maxSpeed)
			.Add("travelDistance", travelDistance)
			.Add("scale", scale)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "count", count);
		Convert::fromJson(jsonData, "spawnCenter", spawnCenter);
		Convert::fromJson(jsonData, "spawnSize", spawnSize);
		Convert::fromJson(jsonData, "fallAngle", fallAngle);
		Convert::fromJson(jsonData, "minSpeed", minSpeed);
		Convert::fromJson(jsonData, "maxSpeed", maxSpeed);
		Convert::fromJson(jsonData, "travelDistance", travelDistance);
		Convert::fromJson(jsonData, "scale", scale);
	}
};

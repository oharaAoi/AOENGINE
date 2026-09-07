#pragma once
#include <cstdint>
#include <string>

#include "Engine/Lib/Color.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

/// <summary>
/// ゲーム開始前のカウントダウン演出の調整値
/// </summary>
struct IntroUIParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// 出す文字の数。3 / 2 / 1 / Go の4つ
	static constexpr std::size_t kItemCount = 4;

	// --- 背景を暗くするスプライト ---
	AOENGINE::Color backColor{ 0.0f, 0.0f, 0.0f, 0.6f };
	Math::Vector2 backCenter{ 960.0f, 540.0f };
	Math::Vector2 backSize{ 1920.0f, 1080.0f };

	// --- 最初に出す目的。背景の箱は持たず、暗転の上に絵をそのまま出す ---
	std::string objectiveTextureName = "white.png";
	Math::Vector2 objectivePos{ 960.0f, 460.0f };
	Math::Vector2 objectiveSize{ 800.0f, 200.0f };

	// シーンが始まってから目的を出すまでの待ち
	float objectiveStartWaitTime;
	// 出るのにかける時間とイージング
	float objectiveInTime;
	int32_t objectiveInEaseKind;
	// 出してから戻すまでの待ち
	float objectiveHoldTime;
	// 戻すのにかける時間とイージング
	float objectiveOutTime;
	int32_t objectiveOutEaseKind;	

	// 目的は位置を動かさず、大きさと回転だけで見せる
	float objectiveScaleStart;
	float objectiveScaleEnd;
	float objectiveRotateStart;
	float objectiveRotateEnd;

	// --- 文字 ---
	float fontSize;
	AOENGINE::Color textColor;
	// 画面上の中心。文字はここで一番大きく見える
	Math::Vector2 centerPos;
	// 出てくる位置と抜けていく位置。どちらも中心からの相対
	Math::Vector2 rightOffset;
	Math::Vector2 leftOffset;

	// --- 右から中心へ ---
	float inTime;
	int32_t inEaseKind;

	// --- 中心から左へ ---
	float outTime;
	int32_t outEaseKind;

	// 中心へ着いてから、次の文字が動き出すまでの間
	float nextOffsetTime = 0.35f;

	// --- 回転(度)。左右に振る動き ---
	// start と end が振れ幅の両端。2つの真ん中へ向かって細くなりながら止まる
	float rotateStart = -25.0f;
	float rotateEnd = 25.0f;
	// 何往復ぶん振るか
	int32_t rotateLoopCount = 3;
	// 振り終わるまでの時間。この時間で指定回数ぶん振って、真ん中へ戻る。
	// 移動のイージングとは別に持たせて、動きの速さを移動と切り離して決められるようにする
	float rotateSwingTime = 0.8f;

	// --- 大きさ。始まり->終わりを loopCount 回ぶん繰り返す ---
	float scaleStart = 2.5f;
	float scaleEnd = 1.0f;
	int32_t scaleLoopCount = 1;

	// --- Go だけの終わり方。左へは行かず、中心で止まってから消える ---
	float goHoldTime = 0.4f;
	float goFadeTime = 0.3f;
	int32_t goFadeEaseKind = 10;	// OutSine
	// 消えきってからイントロを終えるまでの間
	float goEndWaitTime = 0.1f;

	IntroUIParameter() : CustomParameterSet("Intro") {
		SetGroupName("Intro");
		SetName("introUI");

		AddSeparatorText("Back");
		AddParameter("Back Color", backColor);
		AddParameter("Back Center", backCenter, 1.0f);
		AddParameter("Back Size", backSize, 1.0f);

		AddSeparatorText("Objective");
		AddParameter("Objective Texture", objectiveTextureName);
		AddParameter("Objective Pos", objectivePos, 1.0f);
		AddParameter("Objective Size", objectiveSize, 1.0f);
		AddParameter("Objective Start Wait Time", objectiveStartWaitTime, 0.01f, 0.0f, 30.0f);
		AddParameter("Objective In Time", objectiveInTime, 0.01f, 0.0f, 30.0f);
		AddParameter("Objective Hold Time", objectiveHoldTime, 0.01f, 0.0f, 30.0f);
		AddParameter("Objective Out Time", objectiveOutTime, 0.01f, 0.0f, 30.0f);
		AddParameter("Objective Scale Start", objectiveScaleStart, 0.01f, 0.0f, 20.0f);
		AddParameter("Objective Scale End", objectiveScaleEnd, 0.01f, 0.0f, 20.0f);
		AddParameter("Objective Rotate Start(deg)", objectiveRotateStart, 1.0f, -3600.0f, 3600.0f);
		AddParameter("Objective Rotate End(deg)", objectiveRotateEnd, 1.0f, -3600.0f, 3600.0f);

		AddSeparatorText("Text");
		AddParameter("Font Size", fontSize, 1.0f, 1.0f, 500.0f);
		AddParameter("Text Color", textColor);
		AddParameter("Center Pos", centerPos, 1.0f);
		AddParameter("Right Offset", rightOffset, 1.0f);
		AddParameter("Left Offset", leftOffset, 1.0f);

		AddSeparatorText("Move");
		AddParameter("In Time", inTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Out Time", outTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Next Offset Time", nextOffsetTime, 0.01f, 0.0f, 10.0f);

		AddSeparatorText("Rotate");
		AddParameter("Rotate Left(deg)", rotateStart, 1.0f, -3600.0f, 3600.0f);
		AddParameter("Rotate Right(deg)", rotateEnd, 1.0f, -3600.0f, 3600.0f);
		AddParameter("Rotate Swing Count", rotateLoopCount, 1.0f, 0.0f, 20.0f);
		AddParameter("Rotate Swing Time", rotateSwingTime, 0.01f, 0.0f, 10.0f);

		AddSeparatorText("Scale");
		AddParameter("Scale Start", scaleStart, 0.01f, 0.0f, 20.0f);
		AddParameter("Scale End", scaleEnd, 0.01f, 0.0f, 20.0f);
		AddParameter("Scale Loop Count", scaleLoopCount, 1.0f, 1.0f, 20.0f);

		AddSeparatorText("Go");
		AddParameter("Go Hold Time", goHoldTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Go Fade Time", goFadeTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Go End Wait Time", goEndWaitTime, 0.01f, 0.0f, 10.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("backColor", backColor)
			.Add("backCenter", backCenter)
			.Add("backSize", backSize)
			.Add("objectiveTextureName", objectiveTextureName)
			.Add("objectivePos", objectivePos)
			.Add("objectiveSize", objectiveSize)
			.Add("objectiveStartWaitTime", objectiveStartWaitTime)
			.Add("objectiveInTime", objectiveInTime)
			.Add("objectiveInEaseKind", objectiveInEaseKind)
			.Add("objectiveHoldTime", objectiveHoldTime)
			.Add("objectiveOutTime", objectiveOutTime)
			.Add("objectiveOutEaseKind", objectiveOutEaseKind)
			.Add("objectiveScaleStart", objectiveScaleStart)
			.Add("objectiveScaleEnd", objectiveScaleEnd)
			.Add("objectiveRotateStart", objectiveRotateStart)
			.Add("objectiveRotateEnd", objectiveRotateEnd)
			.Add("fontSize", fontSize)
			.Add("textColor", textColor)
			.Add("centerPos", centerPos)
			.Add("rightOffset", rightOffset)
			.Add("leftOffset", leftOffset)
			.Add("inTime", inTime)
			.Add("inEaseKind", inEaseKind)
			.Add("outTime", outTime)
			.Add("outEaseKind", outEaseKind)
			.Add("nextOffsetTime", nextOffsetTime)
			.Add("rotateStart", rotateStart)
			.Add("rotateEnd", rotateEnd)
			.Add("rotateLoopCount", rotateLoopCount)
			.Add("rotateSwingTime", rotateSwingTime)
			.Add("scaleStart", scaleStart)
			.Add("scaleEnd", scaleEnd)
			.Add("scaleLoopCount", scaleLoopCount)
			.Add("goHoldTime", goHoldTime)
			.Add("goFadeTime", goFadeTime)
			.Add("goFadeEaseKind", goFadeEaseKind)
			.Add("goEndWaitTime", goEndWaitTime)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "backColor", backColor);
		Convert::fromJson(jsonData, "backCenter", backCenter);
		Convert::fromJson(jsonData, "backSize", backSize);
		Convert::fromJson(jsonData, "objectiveTextureName", objectiveTextureName);
		Convert::fromJson(jsonData, "objectivePos", objectivePos);
		Convert::fromJson(jsonData, "objectiveSize", objectiveSize);
		Convert::fromJson(jsonData, "objectiveStartWaitTime", objectiveStartWaitTime);
		Convert::fromJson(jsonData, "objectiveInTime", objectiveInTime);
		Convert::fromJson(jsonData, "objectiveInEaseKind", objectiveInEaseKind);
		Convert::fromJson(jsonData, "objectiveHoldTime", objectiveHoldTime);
		Convert::fromJson(jsonData, "objectiveOutTime", objectiveOutTime);
		Convert::fromJson(jsonData, "objectiveOutEaseKind", objectiveOutEaseKind);
		Convert::fromJson(jsonData, "objectiveScaleStart", objectiveScaleStart);
		Convert::fromJson(jsonData, "objectiveScaleEnd", objectiveScaleEnd);
		Convert::fromJson(jsonData, "objectiveRotateStart", objectiveRotateStart);
		Convert::fromJson(jsonData, "objectiveRotateEnd", objectiveRotateEnd);
		Convert::fromJson(jsonData, "fontSize", fontSize);
		Convert::fromJson(jsonData, "textColor", textColor);
		Convert::fromJson(jsonData, "centerPos", centerPos);
		Convert::fromJson(jsonData, "rightOffset", rightOffset);
		Convert::fromJson(jsonData, "leftOffset", leftOffset);
		Convert::fromJson(jsonData, "inTime", inTime);
		Convert::fromJson(jsonData, "inEaseKind", inEaseKind);
		Convert::fromJson(jsonData, "outTime", outTime);
		Convert::fromJson(jsonData, "outEaseKind", outEaseKind);
		Convert::fromJson(jsonData, "nextOffsetTime", nextOffsetTime);
		Convert::fromJson(jsonData, "rotateStart", rotateStart);
		Convert::fromJson(jsonData, "rotateEnd", rotateEnd);
		Convert::fromJson(jsonData, "rotateLoopCount", rotateLoopCount);
		Convert::fromJson(jsonData, "rotateSwingTime", rotateSwingTime);
		Convert::fromJson(jsonData, "scaleStart", scaleStart);
		Convert::fromJson(jsonData, "scaleEnd", scaleEnd);
		Convert::fromJson(jsonData, "scaleLoopCount", scaleLoopCount);
		Convert::fromJson(jsonData, "goHoldTime", goHoldTime);
		Convert::fromJson(jsonData, "goFadeTime", goFadeTime);
		Convert::fromJson(jsonData, "goFadeEaseKind", goFadeEaseKind);
		Convert::fromJson(jsonData, "goEndWaitTime", goEndWaitTime);
	}
};

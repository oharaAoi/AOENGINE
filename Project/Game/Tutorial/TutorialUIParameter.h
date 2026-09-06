#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Lib/Math/Vector2.h"

/// <summary>
/// チュートリアルのUIの調整値
/// </summary>
struct TutorialUIParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// チェックを置ける数。ページの数と合わせる
	static constexpr std::size_t kCheckCount = 5;
	// 1ページに出せるチェックの数。移動のページだけが例外的に2つ使う
	static constexpr std::size_t kCheckSlotCount = 2;

	// --- 枠 ---
	Math::Vector2 boxCenter{ 960.0f, 840.0f };	// 画面上での中心
	Math::Vector2 boxSize{ 1520.0f, 280.0f };	// 枠の大きさ
	float uiScale = 1.0f;						// 全体の拡大率。子も一緒に変わる

	// --- 文字 ---
	// 枠の中心からの相対位置。アンカーはどれも中心
	Math::Vector2 bodyOffset{ 0.0f, -50.0f };
	float bodyFontSize = 40.0f;

	// --- 案内。「次へ」と「戻る」で同じ見た目の枠を2つ出す ---
	// 本体の枠の中心からの相対位置
	Math::Vector2 nextBoxOffset{ 560.0f, 85.0f };
	Math::Vector2 backBoxOffset{ 200.0f, 85.0f };
	Math::Vector2 guideBoxSize{ 340.0f, 70.0f };	// 案内の枠の大きさ
	Math::Vector2 guideOffset{ 0.0f, 0.0f };		// 案内の枠の中心からの文字の相対位置
	float guideFontSize = 28.0f;

	// --- チェック ---
	Math::Vector2 checkSize{ 64.0f, 64.0f };	// チェックの大きさ
	// ページごとの置き場所。枠の中心からの相対位置
	std::array<Math::Vector2, kCheckCount> checkOffset{
		Math::Vector2{ 560.0f, -90.0f },
		Math::Vector2{ 640.0f, -90.0f },
		Math::Vector2{ 640.0f, -90.0f },
		Math::Vector2{ 640.0f, -90.0f },
		Math::Vector2{ 640.0f, -90.0f },
	};
	// 移動のページだけ、ジャンプ用のチェックがもう1つ出る。そのぶんの置き場所
	Math::Vector2 moveJumpCheckOffset{ 680.0f, -90.0f };

	// --- 出し入れの動き ---
	float appearScale = 0.0f;

	float openTime = 0.3f;			// 開くのにかける時間
	int32_t openEaseKind = 13;		// 開く時のイージング種類

	float closeTime = 0.2f;			// 閉じるのにかける時間
	int32_t closeEaseKind = 3;		// 閉じる時のイージング種類

	float guideTime = 0.25f;		// 送る案内が出るのにかける時間
	int32_t guideEaseKind = 17;		// 送る案内のイージング種類

	float checkTime = 0.25f;		// チェックが出るのにかける時間
	int32_t checkEaseKind = 17;		// チェックのイージング種類

	TutorialUIParameter() : CustomParameterSet("TutorialUI") {
		SetGroupName("Tutorial");
		SetName("tutorialUI");

		AddSeparatorText("Box");
		AddParameter("Box Center", boxCenter, 1.0f);
		AddParameter("Box Size", boxSize, 1.0f);
		AddParameter("UI Scale", uiScale, 0.01f, 0.01f, 10.0f);

		AddSeparatorText("Text");
		AddParameter("Body Offset", bodyOffset, 1.0f);
		AddParameter("Body Font Size", bodyFontSize, 1.0f, 1.0f, 200.0f);

		AddSeparatorText("Guide");
		AddParameter("Next Box Offset", nextBoxOffset, 1.0f);
		AddParameter("Back Box Offset", backBoxOffset, 1.0f);
		AddParameter("Guide Box Size", guideBoxSize, 1.0f);
		AddParameter("Guide Offset", guideOffset, 1.0f);
		AddParameter("Guide Font Size", guideFontSize, 1.0f, 1.0f, 200.0f);

		AddSeparatorText("Check");
		AddParameter("Check Size", checkSize, 1.0f);
		AddParameter("Check Offset: Move(移動)", checkOffset[0], 1.0f);
		AddParameter("Check Offset: Move(ジャンプ)", moveJumpCheckOffset, 1.0f);
		AddParameter("Check Offset: DamageFloor", checkOffset[1], 1.0f);
		AddParameter("Check Offset: Connect", checkOffset[2], 1.0f);
		AddParameter("Check Offset: Launch", checkOffset[3], 1.0f);
		AddParameter("Check Offset: Ending", checkOffset[4], 1.0f);

		AddSeparatorText("Appear");
		AddParameter("Appear Scale", appearScale, 0.01f, 0.0f, 10.0f);
		AddParameter("Open Time", openTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Close Time", closeTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Guide Time", guideTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Check Time", checkTime, 0.01f, 0.0f, 10.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("boxCenter", boxCenter)
			.Add("boxSize", boxSize)
			.Add("uiScale", uiScale)
			.Add("bodyOffset", bodyOffset)
			.Add("nextBoxOffset", nextBoxOffset)
			.Add("backBoxOffset", backBoxOffset)
			.Add("guideBoxSize", guideBoxSize)
			.Add("guideOffset", guideOffset)
			.Add("bodyFontSize", bodyFontSize)
			.Add("guideFontSize", guideFontSize)
			.Add("checkSize", checkSize)
			.Add("checkOffset0", checkOffset[0])
			.Add("checkOffset1", checkOffset[1])
			.Add("checkOffset2", checkOffset[2])
			.Add("checkOffset3", checkOffset[3])
			.Add("checkOffset4", checkOffset[4])
			.Add("moveJumpCheckOffset", moveJumpCheckOffset)
			.Add("appearScale", appearScale)
			.Add("openTime", openTime)
			.Add("openEaseKind", openEaseKind)
			.Add("closeTime", closeTime)
			.Add("closeEaseKind", closeEaseKind)
			.Add("guideTime", guideTime)
			.Add("guideEaseKind", guideEaseKind)
			.Add("checkTime", checkTime)
			.Add("checkEaseKind", checkEaseKind)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "boxCenter", boxCenter);
		Convert::fromJson(jsonData, "boxSize", boxSize);
		Convert::fromJson(jsonData, "uiScale", uiScale);
		Convert::fromJson(jsonData, "bodyOffset", bodyOffset);
		Convert::fromJson(jsonData, "nextBoxOffset", nextBoxOffset);
		Convert::fromJson(jsonData, "backBoxOffset", backBoxOffset);
		Convert::fromJson(jsonData, "guideBoxSize", guideBoxSize);
		Convert::fromJson(jsonData, "guideOffset", guideOffset);
		Convert::fromJson(jsonData, "bodyFontSize", bodyFontSize);
		Convert::fromJson(jsonData, "guideFontSize", guideFontSize);
		Convert::fromJson(jsonData, "checkSize", checkSize);
		Convert::fromJson(jsonData, "checkOffset0", checkOffset[0]);
		Convert::fromJson(jsonData, "checkOffset1", checkOffset[1]);
		Convert::fromJson(jsonData, "checkOffset2", checkOffset[2]);
		Convert::fromJson(jsonData, "checkOffset3", checkOffset[3]);
		Convert::fromJson(jsonData, "checkOffset4", checkOffset[4]);
		Convert::fromJson(jsonData, "moveJumpCheckOffset", moveJumpCheckOffset);
		Convert::fromJson(jsonData, "appearScale", appearScale);
		Convert::fromJson(jsonData, "openTime", openTime);
		Convert::fromJson(jsonData, "openEaseKind", openEaseKind);
		Convert::fromJson(jsonData, "closeTime", closeTime);
		Convert::fromJson(jsonData, "closeEaseKind", closeEaseKind);
		Convert::fromJson(jsonData, "guideTime", guideTime);
		Convert::fromJson(jsonData, "guideEaseKind", guideEaseKind);
		Convert::fromJson(jsonData, "checkTime", checkTime);
		Convert::fromJson(jsonData, "checkEaseKind", checkEaseKind);
	}
};

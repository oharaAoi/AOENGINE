#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string>

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
	// 1ページに出せる操作ボタンの数。移動のページがキーボードで3つ使う
	static constexpr std::size_t kButtonSlotCount = 3;

	// --- 枠 ---
	Math::Vector2 boxCenter{ 960.0f, 840.0f };	// 画面上での中心
	Math::Vector2 boxSize{ 1520.0f, 280.0f };	// 枠の大きさ
	float uiScale = 1.0f;						// 全体の拡大率。子も一緒に変わる

	// 文字に使うフォント。ファイル名だけでも Engine/Font から引ける
	std::string fontPath = "./Project/Assets/Engine/Font/Senobi-Gothic-Medium.ttf";

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

	// --- 操作ボタンの画像 ---
	Math::Vector2 buttonSize{ 96.0f, 96.0f };		// ボタン1つの大きさ
	// スペースキーだけ横長なので、そこだけ別の大きさを使う
	Math::Vector2 spaceButtonSize{ 200.0f, 96.0f };

	// 1ページぶんの置き場所。jsonに並べた順で1つずつ持つ
	using ButtonOffsets = std::array<Math::Vector2, kButtonSlotCount>;

	// 枠の中心からの相対位置
	std::array<ButtonOffsets, kCheckCount> buttonKeyboardOffset{};
	std::array<ButtonOffsets, kCheckCount> buttonPadOffset{};
	// 案内の枠の中心から見た、そこに置くボタンの位置と大きさ
	Math::Vector2 guideButtonOffset{ -130.0f, 0.0f };
	Math::Vector2 guideButtonSize{ 56.0f, 56.0f };

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

	float buttonTime = 0.25f;		// 操作ボタンが出るのにかける時間
	int32_t buttonEaseKind = 17;	// 操作ボタンのイージング種類

	// ボタンの置き場所の既定値
	const float kDefaultButtonLeft = -620.0f;
	const float kDefaultButtonTop = 30.0f;
	const float kDefaultButtonPitch = 110.0f;

	TutorialUIParameter() : CustomParameterSet("TutorialUI") {
		SetGroupName("Tutorial");
		SetName("tutorialUI");

		AddSeparatorText("Box");
		AddParameter("Box Center", boxCenter, 1.0f);
		AddParameter("Box Size", boxSize, 1.0f);
		AddParameter("UI Scale", uiScale, 0.01f, 0.01f, 10.0f);

		AddSeparatorText("Text");
		AddParameter("Font Path", fontPath);
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

		AddSeparatorText("Button");
		AddParameter("Button Size", buttonSize, 1.0f);
		AddParameter("Space Button Size", spaceButtonSize, 1.0f);
		AddParameter("Guide Button Offset", guideButtonOffset, 1.0f);
		AddParameter("Guide Button Size", guideButtonSize, 1.0f);

		// 既定は横一列。ここから各ページで動かしてもらう
		for (std::size_t page = 0; page < kCheckCount; ++page) {
			for (std::size_t slot = 0; slot < kButtonSlotCount; ++slot) {
				const Math::Vector2 lined{
					kDefaultButtonLeft + kDefaultButtonPitch * static_cast<float>(slot),
					kDefaultButtonTop
				};
				buttonKeyboardOffset[page][slot] = lined;
				buttonPadOffset[page][slot] = lined;
			}
		}

		// ページごとに、キーボード用とパッド用を並べて出す
		const std::string pageNames[kCheckCount] = {
			"Move", "DamageFloor", "Connect", "Launch", "Ending"
		};

		// 絵を出すページだけ触れるようにする
		const std::size_t buttonPages[] = { 0, 2, 3 };

		for (std::size_t page : buttonPages) {

			AddSeparatorText("Button Offset: " + pageNames[page]);

			for (std::size_t slot = 0; slot < kButtonSlotCount; ++slot) {
				AddParameter("Key " + std::to_string(slot) + " (" + pageNames[page] + ")",
					buttonKeyboardOffset[page][slot], 1.0f);
			}
			for (std::size_t slot = 0; slot < kButtonSlotCount; ++slot) {
				AddParameter("Pad " + std::to_string(slot) + " (" + pageNames[page] + ")",
					buttonPadOffset[page][slot], 1.0f);
			}
		}

		AddSeparatorText("Appear");
		AddParameter("Appear Scale", appearScale, 0.01f, 0.0f, 10.0f);
		AddParameter("Open Time", openTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Close Time", closeTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Guide Time", guideTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Check Time", checkTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Button Time", buttonTime, 0.01f, 0.0f, 10.0f);
	}

	/// <summary>ボタンの置き場所を保存する時の名前を作る</summary>
	static std::string MakeButtonOffsetKey(const std::string& prefix, std::size_t page, std::size_t slot) {
		return prefix + std::to_string(page) + "_" + std::to_string(slot);
	}

	json ToJson(const std::string& id) const override {
		AOENGINE::JsonBuilder builder(id);

		// 数が多いので、ボタンの置き場所だけ回して入れる
		for (std::size_t page = 0; page < kCheckCount; ++page) {
			for (std::size_t slot = 0; slot < kButtonSlotCount; ++slot) {
				builder.Add(MakeButtonOffsetKey("buttonKeyboardOffset", page, slot),
					buttonKeyboardOffset[page][slot]);
				builder.Add(MakeButtonOffsetKey("buttonPadOffset", page, slot),
					buttonPadOffset[page][slot]);
			}
		}

		return builder
			.Add("boxCenter", boxCenter)
			.Add("boxSize", boxSize)
			.Add("uiScale", uiScale)
			.Add("fontPath", fontPath)
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
			.Add("buttonSize", buttonSize)
			.Add("spaceButtonSize", spaceButtonSize)
			.Add("guideButtonOffset", guideButtonOffset)
			.Add("guideButtonSize", guideButtonSize)
			.Add("buttonTime", buttonTime)
			.Add("buttonEaseKind", buttonEaseKind)
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
		Convert::fromJson(jsonData, "fontPath", fontPath);
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
		Convert::fromJson(jsonData, "buttonSize", buttonSize);
		Convert::fromJson(jsonData, "spaceButtonSize", spaceButtonSize);

		for (std::size_t page = 0; page < kCheckCount; ++page) {
			for (std::size_t slot = 0; slot < kButtonSlotCount; ++slot) {
				Convert::fromJson(jsonData, MakeButtonOffsetKey("buttonKeyboardOffset", page, slot),
					buttonKeyboardOffset[page][slot]);
				Convert::fromJson(jsonData, MakeButtonOffsetKey("buttonPadOffset", page, slot),
					buttonPadOffset[page][slot]);
			}
		}
		Convert::fromJson(jsonData, "guideButtonOffset", guideButtonOffset);
		Convert::fromJson(jsonData, "guideButtonSize", guideButtonSize);
		Convert::fromJson(jsonData, "buttonTime", buttonTime);
		Convert::fromJson(jsonData, "buttonEaseKind", buttonEaseKind);
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

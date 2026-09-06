#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// チュートリアルの説明文を持つ表
/// </summary>
class TutorialTextTable {
public:

	TutorialTextTable() = default;
	~TutorialTextTable() = default;

	/// <summary>jsonから文言を読み込む</summary>
	void Load();

	/// <summary>
	/// ページの説明文を引く。
	/// </summary>
	/// <param name="key">ページのキー</param>
	/// <param name="isPadConnected">コントローラーが繋がっているか</param>
	const std::string& GetStepText(const std::string& key, bool isPadConnected) const;

	/// <summary>「次へ送る」操作の案内文を引く</summary>
	const std::string& GetNextGuide(bool isPadConnected) const;

	/// <summary>「前へ戻る」操作の案内文を引く</summary>
	const std::string& GetBackGuide(bool isPadConnected) const;

	/// <summary>
	/// ページで教える操作の、ボタン画像名を引く
	/// </summary>
	/// <param name="key">ページのキー</param>
	/// <param name="isPadConnected">コントローラーが繋がっているか</param>
	const std::vector<std::string>& GetStepButtons(const std::string& key, bool isPadConnected) const;

	/// <summary>「次へ送る」操作のボタン画像名を引く</summary>
	const std::vector<std::string>& GetNextGuideButtons(bool isPadConnected) const;

	/// <summary>「前へ戻る」操作のボタン画像名を引く</summary>
	const std::vector<std::string>& GetBackGuideButtons(bool isPadConnected) const;

private:

	// パッド用とキーボード用の1組
	struct Entry {
		std::string pad;
		std::string keyboard;

		// 説明と一緒に出すボタン画像名。並べた順に左から置かれる
		std::vector<std::string> padButtons;
		std::vector<std::string> keyboardButtons;
	};

	/// <summary>組から、今の接続状況に合う方を返す</summary>
	const std::string& Select(const Entry& entry, bool isPadConnected) const;

	/// <summary>ボタン画像名も同じ決め方で選ぶ</summary>
	const std::vector<std::string>& SelectButtons(const Entry& entry, bool isPadConnected) const;

	/// <summary>jsonの1組を読み込む</summary>
	static Entry ParseEntry(const nlohmann::json& value);

private:

	// ページのキー->説明文
	std::unordered_map<std::string, Entry> steps_;

	// 次へ送る案内 / 前へ戻る案内
	Entry nextGuide_;
	Entry backGuide_;

	// 引けなかった時に返すもの
	const std::string kEmpty_;
	const std::vector<std::string> kEmptyButtons_;

	// 読み込み元
	const std::string kFolderPath = "./Project/Assets/Game/GameData/Tutorial";
	const std::string kFileName = "TutorialText";
};

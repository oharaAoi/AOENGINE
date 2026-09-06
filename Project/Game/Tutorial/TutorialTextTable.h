#pragma once
#include <string>
#include <unordered_map>

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

private:

	// パッド用とキーボード用の1組
	struct Entry {
		std::string pad;
		std::string keyboard;
	};

	/// <summary>組から、今の接続状況に合う方を返す</summary>
	const std::string& Select(const Entry& entry, bool isPadConnected) const;

private:

	// ページのキー->説明文
	std::unordered_map<std::string, Entry> steps_;

	// 次へ送る案内 / 前へ戻る案内
	Entry nextGuide_;
	Entry backGuide_;

	// 引けなかった時に返すもの
	const std::string kEmpty_;

	// 読み込み元
	const std::string kFolderPath = "./Project/Assets/Game/GameData/Tutorial";
	const std::string kFileName = "TutorialText";
};

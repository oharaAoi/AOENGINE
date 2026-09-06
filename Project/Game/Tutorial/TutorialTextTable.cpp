#include "TutorialTextTable.h"

#include "Engine/Lib/Json/JsonSerializer.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextTable::Load() {

	steps_.clear();
	nextGuide_ = Entry{};
	backGuide_ = Entry{};

	const nlohmann::json root = AOENGINE::JsonSerializer::Load(kFolderPath, kFileName);

	// ファイルが無い場合は空のjsonが返る。文言が出ないだけで進行はできるようにしておく
	if (!root.is_object()) {
		return;
	}

	if (root.contains("steps") && root.at("steps").is_object()) {
		for (const auto& [key, value] : root.at("steps").items()) {
			if (!value.is_object()) {
				continue;
			}

			steps_[key] = ParseEntry(value);
		}
	}

	if (root.contains("nextGuide") && root.at("nextGuide").is_object()) {
		nextGuide_ = ParseEntry(root.at("nextGuide"));
	}

	if (root.contains("backGuide") && root.at("backGuide").is_object()) {
		backGuide_ = ParseEntry(root.at("backGuide"));
	}
}

TutorialTextTable::Entry TutorialTextTable::ParseEntry(const nlohmann::json& value) {

	Entry entry{};
	entry.pad = value.value("pad", std::string());
	entry.keyboard = value.value("keyboard", std::string());

	// ボタン画像は無くてもよい。書かれていない時は文字だけの説明になる
	if (value.contains("padButtons") && value.at("padButtons").is_array()) {
		entry.padButtons = value.at("padButtons").get<std::vector<std::string>>();
	}
	if (value.contains("keyboardButtons") && value.at("keyboardButtons").is_array()) {
		entry.keyboardButtons = value.at("keyboardButtons").get<std::vector<std::string>>();
	}

	return entry;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  文言を引く
///////////////////////////////////////////////////////////////////////////////////////////////

const std::string& TutorialTextTable::GetStepText(const std::string& key, bool isPadConnected) const {

	auto it = steps_.find(key);
	if (it == steps_.end()) {
		return kEmpty_;
	}

	return Select(it->second, isPadConnected);
}

const std::string& TutorialTextTable::GetNextGuide(bool isPadConnected) const {
	return Select(nextGuide_, isPadConnected);
}

const std::string& TutorialTextTable::GetBackGuide(bool isPadConnected) const {
	return Select(backGuide_, isPadConnected);
}

const std::vector<std::string>& TutorialTextTable::GetStepButtons(
	const std::string& key, bool isPadConnected) const {

	auto it = steps_.find(key);
	if (it == steps_.end()) {
		return kEmptyButtons_;
	}

	return SelectButtons(it->second, isPadConnected);
}

const std::vector<std::string>& TutorialTextTable::GetNextGuideButtons(bool isPadConnected) const {
	return SelectButtons(nextGuide_, isPadConnected);
}

const std::vector<std::string>& TutorialTextTable::GetBackGuideButtons(bool isPadConnected) const {
	return SelectButtons(backGuide_, isPadConnected);
}

const std::string& TutorialTextTable::Select(const Entry& entry, bool isPadConnected) const {

	// コントローラーが繋がっていない場合はキーボード操作の説明に切り替える
	if (!isPadConnected) {
		return entry.keyboard;
	}

	// パッド用が用意されていなければキーボード用で代用する
	if (entry.pad.empty()) {
		return entry.keyboard;
	}

	return entry.pad;
}

const std::vector<std::string>& TutorialTextTable::SelectButtons(
	const Entry& entry, bool isPadConnected) const {

	// 文字と同じで、繋がっていなければキーボード用
	if (!isPadConnected || entry.padButtons.empty()) {
		return entry.keyboardButtons;
	}

	return entry.padButtons;
}

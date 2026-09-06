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

			Entry entry{};
			entry.pad = value.value("pad", std::string());
			entry.keyboard = value.value("keyboard", std::string());
			steps_[key] = entry;
		}
	}

	if (root.contains("nextGuide") && root.at("nextGuide").is_object()) {
		const nlohmann::json& guide = root.at("nextGuide");
		nextGuide_.pad = guide.value("pad", std::string());
		nextGuide_.keyboard = guide.value("keyboard", std::string());
	}

	if (root.contains("backGuide") && root.at("backGuide").is_object()) {
		const nlohmann::json& guide = root.at("backGuide");
		backGuide_.pad = guide.value("pad", std::string());
		backGuide_.keyboard = guide.value("keyboard", std::string());
	}
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

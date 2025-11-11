#include "ShaderGraphSerializer.h"
#include <iostream>

bool ShaderGraphSerializer::Save(const std::string& _direPath, const std::string& _fileName, ImFlow::ImNodeFlow& _editor) {
	
	// -------------------------------------------------
		// ↓ ディレクトリがなければ作成を行う
		// -------------------------------------------------
	if (!std::filesystem::exists(_direPath)) {
		std::filesystem::create_directories(_direPath);
		std::cout << "Created directory: " << _direPath << std::endl;
	}
	
	// -------------------------------------------------
	// ↓ ファイルを開けるかのチェックを行う
	// -------------------------------------------------
	std::ofstream outFile(_direPath + _fileName);
	if (outFile.fail()) {
		std::string message = "Faild open data file for write\n";
		assert(0);
		return false;
	}

	// -------------------------------------------------
	// ↓ jsonに登録
	// -------------------------------------------------
    // jsonを作成
    json result;

    // nodeを保存
    for (auto& node : _editor.getNodes()) {
        result["nodes"].push_back(node.second.get()->toJson());
    }

	for (auto& weakLink : _editor.getLinks()) {
		if (weakLink.expired()) continue;
		auto link = weakLink.lock();

		json jsonLink;
		jsonLink["fromNode"] = link->left()->getParent()->getUID();
		jsonLink["fromPin"] = link->left()->getName();
		jsonLink["toNode"] = link->right()->getParent()->getUID();
		jsonLink["toPin"] = link->right()->getName();

		result["links"].push_back(jsonLink);
	}

	// -------------------------------------------------
	// ↓ ファイルに実際に書き込む
	// -------------------------------------------------
	outFile << std::setw(4) << result << std::endl;
	outFile.close();

    return true;
}

json ShaderGraphSerializer::Load(const std::string& _direPath, const std::string& _fileName) {
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(_direPath + _fileName);

	json root;
	if (ifs.fail()) {
		std::string message = "not Exist " + (_direPath + _fileName) + ".json";
		assert(0);
		return root;
	}

	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();
	return root;
}

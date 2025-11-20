#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/// <summary>
/// Jsonの情報をまとめたクラス
/// </summary>
class JsonItems {
public:

	struct Group {
		std::unordered_map<std::string, json> items;
	};

	struct ConverterGroup {
		std::unordered_map<std::string, std::function<json(const std::string&)>> items;
		std::string key;
	};

public: // コンストラクタ

	JsonItems() = default;
	~JsonItems() = default;

	// シングルトン化
	JsonItems(const JsonItems&) = delete;
	JsonItems& operator=(const JsonItems&) = delete;
	static JsonItems* GetInstance();

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="nowScene">: 現在のシーン</param>
	void Init(const std::string& _nowScene);

	/// <summary>
	/// jsonファイルに保存する
	/// </summary>
	/// <param name="groupName">: ファイルを保存するフォルダ名</param>
	/// <param name="saveData">: 保存するデータ</param>
	static void Save(const std::string& _groupName, const json& _saveData, const std::string& _rootFold = "");

	/// <summary>
	/// jsonファイルを読み込む
	/// </summary>
	/// <param name="groupName">: ファイルの保存されたフォルダ名</param>
	/// <param name="rootKey">: 読み込むファイル名</param>
	static void Load(const std::string& _groupName, const std::string& _rootKey, const std::string& _rootFold = "");

	/// <summary>
	/// 値の取得(他クラスで呼び出し用)
	/// </summary>
	/// <param name="groupName">: ファイルの保存されたフォルダ名</param>
	/// <param name="rootKey">: 取得するファイル名</param>
	/// <returns>json型を返す</returns>
	static json GetData(const std::string& _groupName, const std::string& _rootKey);

	/// <summary>
	/// コンバーターの関数を保存しておく
	/// </summary>
	/// <param name="groupName">: フォルダ名</param>
	/// <param name="rootKey">: ファイル名</param>
	/// <param name="function">: converterの関数</param>
	static void AddConverter(const std::string& _groupName, const std::string& _rootKey, std::function<json(const std::string&)> _function);

	/// <summary>
	/// すべてのファイルを読み込む
	/// </summary>
	void LoadAllFile();

	/// <summary>
	/// すべてのデータを保存
	/// </summary>
	void SaveAllFile();

private :

	/// <summary>
	/// json項目を追加する
	/// </summary>
	/// <param name="groupName">: ファイルを保存するフォルダ名</param>
	/// <param name="jsonData">: 保存するデータ</param>
	void AddGroup(const std::string& _groupName, const json& _jsonData);

	/// <summary>
	/// 値の取得(自クラスのみで呼び出し)
	/// </summary>
	/// <param name="groupName">: ファイルの保存されたフォルダ名</param>
	/// <param name="rootKey">: 取得するファイル名</param>
	/// <returns>json型を返す</returns>
	json GetValue(const std::string& _groupName, const std::string& _rootKey);


	void AddConverterGroup(const std::string& _groupName, const std::string& _rootKey, std::function<json(const std::string&)> _function);

private:
	
	static const std::string sDirectoryPath_;
	static std::string sNowSceneName_;
	
	std::unordered_map<std::string, Group> jsonMap_;

	std::unordered_map<std::string, ConverterGroup> jsonConverterMap_;
};

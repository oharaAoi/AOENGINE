#include "LineLoader.h"

/// engine
#include "Engine/Utilities/Logger.h"

/// stl
#include <fstream>

using namespace AOENGINE;

namespace{
	/// 見つからなかった場合に返す空の配列
	const std::vector<Math::Vector3> kEmptyPoints{};
}

LineLoader* LineLoader::GetInstance(){
	static LineLoader instance;
	return &instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LineLoader::Init(){
	lineMap_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Lineファイルの読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LineLoader::Load(const std::string& directory, const std::string& fileName, const std::string& extension){
	const std::string fullPath = directory + fileName + extension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullPath);
	// 開けたかのチェック
	if(file.fail()){
		Logger::AssertLog("line fileを開けませんでした : " + fullPath);
		return;
	}

	// fileの中身をjsonに書き出す
	json deserialized;
	// 解凍
	file >> deserialized;

	if(!deserialized.is_object()){
		Logger::AssertLog("line fileの形式が不正です : " + fullPath);
		return;
	}

	// -------------------------------------------------
	// ↓ 親のkeyがLineのObjectNameになっている
	// -------------------------------------------------

	for(auto it = deserialized.begin(); it != deserialized.end(); ++it){
		const std::string& name = it.key();

		if(!it.value().is_object()){
			continue;
		}

		lineMap_[name] = LoadLine(name, it.value());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Lineの読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

LineLoader::LineData LineLoader::LoadLine(const std::string& name, const json& lineJson){
	LineData lineData;
	lineData.name = name;

	if(lineJson.contains("sampling")){
		lineData.sampling = lineJson["sampling"].get<float>();
	}

	if(lineJson.contains("length")){
		lineData.length = lineJson["length"].get<float>();
	}

	if(lineJson.contains("closed")){
		lineData.isClosed = lineJson["closed"].get<bool>();
	}

	// どの保存方式で書き出されたかを保持しておく(変換はしない)
	if(lineJson.contains("axis")){
		lineData.axis = lineJson["axis"].get<std::string>();
	}

	if(lineJson.contains("space")){
		lineData.space = lineJson["space"].get<std::string>();
	}

	if(lineJson.contains("points")){
		lineData.points = ToPoints(lineJson["points"]);
	}

	return lineData;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonからstd::vector<Math::Vector3>への変換
///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Math::Vector3> LineLoader::ToPoints(const json& pointsJson){
	std::vector<Math::Vector3> points;

	if(!pointsJson.is_array()){
		return points;
	}

	points.reserve(pointsJson.size());

	for(const json& pointJson : pointsJson){
		if(!pointJson.contains("x") || !pointJson.contains("y") || !pointJson.contains("z")){
			continue;
		}

		points.emplace_back(pointJson["x"].get<float>(),
							pointJson["y"].get<float>(),
							pointJson["z"].get<float>());
	}

	return points;
}

std::vector<Math::Vector3> LineLoader::LoadPoints(const std::string& filePath, const std::string& lineName){
	std::ifstream file;
	file.open(filePath);
	if(file.fail()){
		Logger::AssertLog("line fileを開けませんでした : " + filePath);
		return {};
	}

	json deserialized;
	file >> deserialized;

	if(!deserialized.is_object() || !deserialized.contains(lineName)){
		Logger::AssertLog("lineが存在しません : " + lineName);
		return {};
	}

	const json& lineJson = deserialized[lineName];
	if(!lineJson.contains("points")){
		return {};
	}

	return ToPoints(lineJson["points"]);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込んだLineの取得
///////////////////////////////////////////////////////////////////////////////////////////////

const std::vector<Math::Vector3>& LineLoader::GetPoints(const std::string& lineName) const{
	auto it = lineMap_.find(lineName);
	if(it == lineMap_.end()){
		Logger::Log("not contains lineData : " + lineName);
		return kEmptyPoints;
	}

	return it->second.points;
}

const LineLoader::LineData* LineLoader::GetLineData(const std::string& lineName) const{
	auto it = lineMap_.find(lineName);
	if(it == lineMap_.end()){
		return nullptr;
	}

	return &it->second;
}

std::vector<std::string> LineLoader::GetLineNames() const{
	std::vector<std::string> names;
	names.reserve(lineMap_.size());

	for(const auto& [name, lineData] : lineMap_){
		names.push_back(name);
	}

	return names;
}

bool LineLoader::Contains(const std::string& lineName) const{
	return lineMap_.find(lineName) != lineMap_.end();
}

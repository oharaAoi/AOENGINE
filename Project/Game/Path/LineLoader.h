#pragma once

/// stl
#include <string>
#include <vector>
#include <unordered_map>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Lib/Math/Vector3.h"

using json = nlohmann::json;

/// <summary>
/// Blenderのアドオンで書き出したLineのサンプリング座標を読み込むクラス。
/// 座標系(保存方式)はアドオン側で確定しているため、ここでは一切変換をしない
/// </summary>
class LineLoader{
public:

	/// <summary>Line1本分の情報</summary>
	struct LineData{
		std::string name;						// LineのObjectName
		float sampling = 0.0f;					// サンプリングした間隔
		float length = 0.0f;					// Lineの全長
		bool isClosed = false;					// 閉じたLineか
		std::string axis;						// 保存された座標系 (例: "+X,+Z,+Y")
		std::string space;						// "world" or "local"
		std::vector<Math::Vector3> points;		// サンプリングした座標
	};

	LineLoader() = default;
	~LineLoader() = default;
	LineLoader(const LineLoader&) = delete;
	const LineLoader& operator=(const LineLoader&) = delete;

	static LineLoader* GetInstance();

	/// <summary>
	/// 読み込んだLineを全て破棄する
	/// </summary>
	void Init();

	/// <summary>
	/// Lineファイルの読み込み処理。ファイル内の全Lineを名前付きで保持する
	/// </summary>
	/// <param name="directory">: ディレクトリ</param>
	/// <param name="fileName">: ファイル名</param>
	/// <param name="extension">: 拡張子</param>
	void Load(const std::string& directory, const std::string& fileName, const std::string& extension = ".json");

	/// <summary>
	/// Line1本分の読み込み処理
	/// </summary>
	/// <param name="name">: LineのObjectName</param>
	/// <param name="lineJson">: Line1本分のjson</param>
	LineData LoadLine(const std::string& name, const json& lineJson);

	/// <summary>
	/// jsonの点配列をstd::vector<Math::Vector3>へ変換する。
	/// 値はファイルに保存されたまま格納する(座標系の変換はしない)
	/// </summary>
	/// <param name="pointsJson">: { "x", "y", "z" } の配列</param>
	static std::vector<Math::Vector3> ToPoints(const json& pointsJson);

	/// <summary>
	/// ファイルから直接座標を取得する(読み込み結果を保持しない)
	/// </summary>
	/// <param name="filePath">: ファイルパス</param>
	/// <param name="lineName">: LineのObjectName</param>
	static std::vector<Math::Vector3> LoadPoints(const std::string& filePath, const std::string& lineName);

public:

	/// <summary>
	/// 指定したLineの座標を取得する。無い場合は空の配列を返す
	/// </summary>
	/// <param name="lineName">: LineのObjectName</param>
	const std::vector<Math::Vector3>& GetPoints(const std::string& lineName) const;

	/// <summary>
	/// 指定したLineの情報を取得する。無い場合はnullptrを返す
	/// </summary>
	/// <param name="lineName">: LineのObjectName</param>
	const LineData* GetLineData(const std::string& lineName) const;

	/// <summary>
	/// 読み込んだLineの名前を取得する
	/// </summary>
	std::vector<std::string> GetLineNames() const;

	/// <summary>
	/// 指定したLineが読み込まれているか
	/// </summary>
	/// <param name="lineName">: LineのObjectName</param>
	bool Contains(const std::string& lineName) const;

	const std::unordered_map<std::string, LineData>& GetLineMap() const { return lineMap_; }

private:

	std::unordered_map<std::string, LineData> lineMap_;

};

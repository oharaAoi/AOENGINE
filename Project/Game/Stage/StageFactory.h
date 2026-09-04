#pragma once

// stl
#include <vector>
#include <queue>
#include <string>

// game
#include "Game/Stage/StageBlockField.h"
#include "Game/Stage/StageSegment.h"

/// <summary>
/// ステージ自動を生産するためのクラス
/// </summary>
class StageFactory{
public:
	static StageFactory& GetInstance();

	/// <summary>
	/// ゲーム開始時の最初のセグメントを読み込む
	/// </summary>
	/// <param name="filePath"></param>
	void LoadStartSegment(const std::string& filePath);

	/// <summary>
	/// 指定されたフォルダー内のcsvファイルを全て segmentとして読み込む
	/// </summary>
	/// <param name="folderPath">csvファイルが格納されているフォルダーのパス</param>
	void LoadSegmentsFromFolder(const std::string& folderPath);

	/// <summary>
	/// ランダムな順番でセグメントを生成する
	/// </summary>
	/// <param name="field">セグメントを跨いでブロックを管理する連結グループ表</param>
	/// <param name="index">ゲームが始まってから何度目のセグメントを生成するか</param>
	void CreateFromRandom(StageBlockField* field,int index);

	void CreateStartSegment(StageBlockField* field,int index = 0){
		field->BuildSegment(startSegment_,index);
	}

	/// <summary>
	/// 指定されたセグメントのブロックを生成する
	/// </summary>
	/// <param name="field"></param>
	/// <param name="index"></param>
	/// <param name="segmentIndex"></param>
	void CreateFromSegment(StageBlockField* field,const StageSegment& segment,int index);

private:
	/// <summary>
	/// セグメントを生成する順番をランダム化する
	/// </summary>
	void RandomiseSegmentIndexQueue();

	StageFactory() = default;
	StageFactory(const StageFactory&) = delete;
	StageFactory& operator=(const StageFactory&) = delete;

private:
	// ゲーム開始時のSegmentは固定のものを使うため、最初のセグメントを保持しておく
	StageSegment startSegment_;
	// セグメントのデータ表
	std::vector<StageSegment> segments_;
	// どの順番でセグメントを生成するかのランダム化されたインデックス
	std::queue<int> randmisedIndexQueue_;
};


#include "StageFactory.h"

/// stl
#include <filesystem>

/// Math
#include "Engine/Lib/Math/MyRandom.h"

StageFactory& StageFactory::GetInstance(){
	static StageFactory instance;
	return instance;
}

void StageFactory::LoadStartSegment(const std::string& filePath){
	startSegment_.LoadBlockData(filePath);
}

void StageFactory::LoadSegmentsFromFolder(const std::string& folderPath){
	// フォルダ内のcsvファイルを全て読み込む
	for(const auto& entry : std::filesystem::directory_iterator(folderPath)){
		if(entry.is_regular_file() && entry.path().extension() == ".csv"){
			StageSegment segment;
			segment.LoadBlockData(entry.path().string());
			segments_.push_back(segment);
		}
	}
}

void StageFactory::CreateFromRandom(StageBlockField* field,int index){
	// ランダム化されたインデックスのキューが空なら、再度ランダム化してキューに詰める
	if(randmisedIndexQueue_.empty()){
		RandomiseSegmentIndexQueue();
	}

	int segmentIndex = randmisedIndexQueue_.front();
	randmisedIndexQueue_.pop();
	field->BuildSegment(segments_[segmentIndex],index);
}

void StageFactory::CreateFromSegment(StageBlockField* field,const StageSegment& segment,int index){
	field->BuildSegment(segment,index);
}

void StageFactory::RandomiseSegmentIndexQueue(){
	// nBag * segments_.size() の数だけ、セグメントのインデックスをランダム化してキューに詰める
	constexpr int nBag = 3;

	// ランダム化のためのインデックス配列を作る
	int segmentCount = static_cast<int>(segments_.size());
	int queueSize = segmentCount * nBag;
	std::vector<int> indices(queueSize);

	// 0,1,2,...,queueSize-1 の順に初期化
	for(int i = 0; i < nBag; i++){
		for(int j = 0; j < segmentCount; j++){
			indices[i * segmentCount + j] = j;
		}
	}

	// インデックスをランダム化
	std::shuffle(indices.begin(),indices.end(),randomEngine);

	// ランダム化されたインデックスをキューに詰める
	for(auto index : indices){
		randmisedIndexQueue_.push(index);
	}
}

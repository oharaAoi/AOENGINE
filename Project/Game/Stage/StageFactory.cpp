#include "StageFactory.h"

void StageFactory::Create(StageBlockField* field, StageSegment* segment, int index, int addHeight) {
	// ココで今生成するcsvのファイル名を作成する
	segment->LoadBlockData("./Project/Assets/Game/StageData/test.csv");
	segment->SetupSegmentOnWorld(field, index, addHeight);
}

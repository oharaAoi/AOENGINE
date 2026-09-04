#include "StageSegment.h"

/// externals
#include "rapidcsv/rapidcsv.h"

void StageSegment::LoadBlockData(const std::string& filePath){
	// CSVからブロックの配置データを読み込む
	// ラベル行・列は無しで読み込む (－1,ー1 でラベルを無効化)
	rapidcsv::Document doc(filePath,rapidcsv::LabelParams(-1,-1));

	for(int i = 0; i < kBlockRow; ++i){
		for(int j = 0; j < kBlockCol; ++j){
			blockData_[i][j] = doc.GetCell<int>(j,i);
		}
	}
}

int StageSegment::GetCell(int row,int col) const{
	if(row < 0 || row >= kBlockRow || col < 0 || col >= kBlockCol){
		return 0;
	}
	return blockData_[row][col];
}

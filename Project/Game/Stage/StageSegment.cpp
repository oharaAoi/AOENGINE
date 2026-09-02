#include "StageSegment.h"

///engine
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

/// externals
#include "rapidcsv/rapidcsv.h"

using namespace AOENGINE;

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

void StageSegment::SetupSegmentOnWorld(){
	constexpr int isBlock = 1; // ブロックが存在することを示す値（仮定）
	constexpr float kBlockSize = 3.0f; // ブロックのサイズ（仮定）

	// とりあえず、原点のまま考える。
	for(int i = 0; i < kBlockRow; ++i){
		float y = i * kBlockSize; // ブロックのY座標を計算

		for(int j = 0; j < kBlockCol; ++j){
			if(blockData_[i][j] == isBlock){
				AOENGINE::SceneObject* root =
					AOENGINE::PrefabManager::GetInstance()->Instantiate("Block");

				BaseGameObject* block = dynamic_cast<BaseGameObject*>(root);
				if(block == nullptr){
					continue;
				}

				// ブロックの位置を設定する
				float x = j * kBlockSize;

				block->GetTransform()->SetTranslate(Math::Vector3(x,y,0.0f));
			}
		}
	}

}

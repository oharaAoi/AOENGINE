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
	constexpr float kBlockSize = 1; // ブロックのサイズ（仮定）

	// セグメント全体の中心が原点(0,0,0)になるように、中央からのオフセットを求める
	constexpr float kOffsetX = (kBlockCol - 1) * 0.5f; // 列方向の中心インデックス
	constexpr float kOffsetY = (kBlockRow - 1) * 0.5f; // 行方向の中心インデックス

	for(int i = 0; i < kBlockRow; ++i){
		// CSVは上の行ほど画面上側を表すため、行インデックスを反転させて +Y に対応させる
		float y = (kOffsetY - i) * kBlockSize; // ブロックのY座標を計算

		for(int j = 0; j < kBlockCol; ++j){
			if(blockData_[i][j] == isBlock){
				AOENGINE::SceneObject* root =
					AOENGINE::PrefabManager::GetInstance()->Instantiate("Block");

				BaseGameObject* block = dynamic_cast<BaseGameObject*>(root);
				if(block == nullptr){
					continue;
				}

				// ブロックの位置を設定する
				float x = (j - kOffsetX) * kBlockSize;

				block->GetTransform()->SetTranslate(Math::Vector3(x,y,0.0f));
			}
		}
	}

}

#include "StageSegment.h"

///engine
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

/// game
#include "Game/Stage/GridPos.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"

/// externals
#include "rapidcsv/rapidcsv.h"

using namespace AOENGINE;

StageSegment::StageSegment() = default;
StageSegment::~StageSegment() = default;
StageSegment::StageSegment(StageSegment&&) noexcept = default;
StageSegment& StageSegment::operator=(StageSegment&&) noexcept = default;

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

void StageSegment::SetupSegmentOnWorld(StageBlockField* field,int segmentOriginGx, int addHeight){
	constexpr int isBlock = 1; // ブロックが存在することを示す値（仮定）

	if(field == nullptr){
		return;
	}

	for(int i = 0; i < kBlockRow; ++i){
		for(int j = 0; j < kBlockCol; ++j){
			if(blockData_[i][j] != isBlock){
				continue;
			}

			// セグメント i行 j列 -> グローバルグリッド座標へ変換
			// CSVは上の行ほど画面上側を表すため、行インデックスを反転させる
			GridPos pos{};
			pos.x = j;
			pos.y = ((kBlockRow - 1) - i) + addHeight;

			// プレハブから Block の実体となる GameObject を生成する
			AOENGINE::SceneObject* root =
				AOENGINE::PrefabManager::GetInstance()->Instantiate("Block");

			AOENGINE::BaseGameObject* gameObject = dynamic_cast<AOENGINE::BaseGameObject*>(root);
			if(gameObject == nullptr){
				continue;
			}

			// Block を生成し、生成した GameObject と関連付ける（所有権はこのセグメントが持つ）
			std::unique_ptr<Block> block = std::make_unique<Block>();
			block->Bind(gameObject);

			// グリッド座標から求めたワールド座標を設定する
			block->GetTransform()->SetTranslate(StageBlockField::GridToWorld(pos));

			// 連結グループ表に登録する（4近傍との連結もここで解決される）
			field->AddBlock(block.get(),pos);

			blocks_.push_back(std::move(block));
		}
	}
}

void StageSegment::UnregisterFromWorld(StageBlockField* field){
	// このセグメントが所有する Block を field の表から外し、GameObject を破棄する。
	// セグメント破棄は連結グループの一部だけを消すことになるが、
	// 画面外（ストリーミングで既に見えなくなった範囲）でのみ行われるため、
	// 残りの連結性についての分割(split)検査はここでは行わない。
	for(std::unique_ptr<Block>& block : blocks_){
		if(block == nullptr){
			continue;
		}
		if(field != nullptr){
			field->RemoveBlockFromField(block.get());
		}
		block->Destroy();
	}

	blocks_.clear();
}

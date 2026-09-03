#include "StageSegment.h"

/// engine
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

/// game
#include "Game/Stage/GridPos.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"
#include "Game/WorldObject/Wall.h"

/// externals
#include "rapidcsv/rapidcsv.h"

using namespace AOENGINE;

namespace
{
	/// CSV上でBlockを表す値
	constexpr int kBlockCell = 1;
	/// CSV上でWallを表す値
	constexpr int kWallCell = 2;

	/// <summary>Prefabから BaseGameObject を生成する。生成できなければ nullptr</summary>
	AOENGINE::BaseGameObject* InstantiateStageObject(const std::string& prefabName){
		AOENGINE::SceneObject* root =
			AOENGINE::PrefabManager::GetInstance()->Instantiate(prefabName);

		return dynamic_cast<AOENGINE::BaseGameObject*>(root);
	}
}

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

void StageSegment::SetupSegmentOnWorld(StageBlockField* field,int segmentIndex){
	if(field == nullptr){
		return;
	}

	for(int i = 0; i < kBlockRow; ++i){
		int y = ((kBlockRow - 1) - i) + segmentIndex * kBlockRow; // CSVは上の行ほど画面上側を表すため、行インデックスを反転させる
		for(int j = 0; j < kBlockCol; ++j){
			const int cell = blockData_[i][j];
			if(cell != kBlockCell && cell != kWallCell){
				continue;
			}

			// セグメント i行 j列 -> グローバルグリッド座標へ変換
			GridPos pos{};
			pos.x = j;
			pos.y = y;

			if(cell == kWallCell){
				CreateWall(pos);
			} else{
				CreateBlock(field,pos);
			}
		}
	}
}

void StageSegment::CreateBlock(StageBlockField* field,const GridPos& pos){
	// プレハブから Block の実体となる GameObject を生成する
	AOENGINE::BaseGameObject* gameObject = InstantiateStageObject("Block");
	if(gameObject == nullptr){
		return;
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

void StageSegment::CreateWall(const GridPos& pos){
	// プレハブから Wall の実体となる GameObject を生成する
	AOENGINE::BaseGameObject* gameObject = InstantiateStageObject("Wall");
	if(gameObject == nullptr){
		return;
	}

	std::unique_ptr<Wall> wall = std::make_unique<Wall>();
	wall->Bind(gameObject);
	wall->SetGridPos(pos);

	// 足場としての位置はBlockと同じ座標系で求める
	wall->GetTransform()->SetTranslate(StageBlockField::GridToWorld(pos));

	// Wall は連結・打ち上げの対象にしないため field には登録しない
	walls_.push_back(std::move(wall));
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

	// Wall は field に登録していないため、GameObject を破棄するだけでよい
	for(std::unique_ptr<Wall>& wall : walls_){
		if(wall == nullptr){
			continue;
		}
		wall->Destroy();
	}

	walls_.clear();
}

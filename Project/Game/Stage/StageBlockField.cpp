#include "StageBlockField.h"

/// game
#include "Game/WorldObject/Block.h"
#include "Game/WorldObject/Wall.h"
#include "Game/EventHandlers/PlayerBlockCollisionCallBacks.h"
#include "Game/Stage/BlockAutoTile.h"

/// engine
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/MyMath.h"

/// stl
#include <algorithm>
#include <cmath>

using namespace AOENGINE;

namespace
{
	/// <summary>Prefabから BaseGameObject を生成する。生成できなければ nullptr</summary>
	AOENGINE::BaseGameObject* InstantiateStageObject(const std::string& prefabName){
		AOENGINE::SceneObject* root =
			AOENGINE::PrefabManager::GetInstance()->Instantiate(prefabName);

		return dynamic_cast<AOENGINE::BaseGameObject*>(root);
	}

	/// <summary>
	/// Wallをオートタイルの隣接判定で「埋まっている」扱いにするかどうか。
	/// falseにすると、Wallに隣接しているだけのBlockは露出扱い(繋がっていない扱い)になる。
	/// ここを1箇所変えるだけで挙動を切り替えられるようにしてある。
	/// </summary>
	constexpr bool kTreatWallAsConnected = true;

	/// <summary>
	/// オートタイルのZ軸回転の符号。
	/// エンジンの回転の向き(左手系/右手系)によっては見た目が反転することがあるため、
	/// ここを-1.0fにするだけで全パターンの回転向きを反転できるようにしてある。
	/// </summary>
	constexpr float kTileRotateSign = 1.0f;

	/// <summary>オートタイル用モデル(2×2×2サイズ)を1マスに合わせるための縮小率</summary>
	constexpr float kTileModelScale = 0.5f;

	/// <summary>
	/// オートタイル用モデルを縮小しても当たり判定が1マス(1×1×1)のままになるようにするコライダーサイズ。
	/// BoxCollider::Update() が transform の scale をAABBに掛けるため、縮小率の逆数を持たせて打ち消す。
	/// </summary>
	constexpr float kTileColliderSize = 1.0f / kTileModelScale;

	/// <summary>
	/// Block prefab の既定色。
	/// SetObject() でマテリアルが作り直された際、退避しておいた色が取得できない場合の
	/// フォールバックとして使う。
	/// </summary>
	const AOENGINE::Color kDefaultBlockColor(1.0f,0.55f,0.15f,1.0f);
}

/// <summary>4近傍（上下左右）のオフセット</summary>
const GridPos StageBlockField::kNeighborOffsets[4] = {
	{1,0},
	{-1,0},
	{0,1},
	{0,-1},
};

StageBlockField::StageBlockField() = default;
StageBlockField::~StageBlockField() = default;

void StageBlockField::AddBlock(Block* block,const GridPos& pos){
	if(block == nullptr){
		return;
	}

	// 同じ座標に既にブロックが存在する場合は登録しない。
	// （セグメント原点の計算ミスなどでしか起きないはずの状態。
	//   黙って上書きすると cells_ と groups_ が食い違うため、ここで弾く）
	if(cells_.find(pos) != cells_.end()){
		return;
	}

	// 1. セル表とブロック自身にグリッド座標を登録する
	cells_[pos] = block;
	block->SetGridPos(pos);

	// 2. 4近傍を調べ、存在するブロックのグループIDを重複なく集める
	std::vector<int> neighborGroupIds;
	for(const GridPos& offset : kNeighborOffsets){
		GridPos neighborPos{pos.x + offset.x,pos.y + offset.y};

		auto it = cells_.find(neighborPos);
		if(it == cells_.end() || it->second == nullptr){
			continue;
		}

		int neighborGroupId = it->second->GetGroupId();
		if(neighborGroupId == kInvalidGroupId){
			continue;
		}

		if(std::find(neighborGroupIds.begin(),neighborGroupIds.end(),neighborGroupId) == neighborGroupIds.end()){
			neighborGroupIds.push_back(neighborGroupId);
		}
	}

	if(neighborGroupIds.empty()){
		// 3. 隣接グループが無い場合は新しいグループを発行して単独グループを作る
		int newGroupId = nextGroupId_;
		++nextGroupId_;

		std::vector<Block*>& members = groups_[newGroupId];
		block->SetGroupId(newGroupId);
		block->SetGroupIndex(static_cast<int>(members.size()));
		members.push_back(block);
		return;
	}

	// 4. 最もメンバー数が多いグループを採用先として選ぶ
	int mainGroupId = neighborGroupIds.front();
	size_t mainGroupSize = groups_[mainGroupId].size();
	for(int groupId : neighborGroupIds){
		size_t size = groups_[groupId].size();
		if(size > mainGroupSize){
			mainGroupId = groupId;
			mainGroupSize = size;
		}
	}

	// 自分自身を採用先グループへ追加する
	std::vector<Block*>& mainMembers = groups_[mainGroupId];
	block->SetGroupId(mainGroupId);
	block->SetGroupIndex(static_cast<int>(mainMembers.size()));
	mainMembers.push_back(block);

	// 5. 残りの隣接グループを採用先グループへ weighted union でマージする
	for(int groupId : neighborGroupIds){
		if(groupId == mainGroupId){
			continue;
		}
		MergeGroup(groupId,mainGroupId);
	}
}

void StageBlockField::MergeGroup(int fromId,int toId){
	if(fromId == toId){
		return;
	}

	auto fromIt = groups_.find(fromId);
	if(fromIt == groups_.end()){
		return;
	}

	// weighted union: 小さい方（from）のメンバーを大きい方（to）へ移し替える
	std::vector<Block*>& toMembers = groups_[toId];
	std::vector<Block*>& fromMembers = fromIt->second;

	for(Block* member : fromMembers){
		if(member == nullptr){
			continue;
		}
		member->SetGroupId(toId);
		member->SetGroupIndex(static_cast<int>(toMembers.size()));
		toMembers.push_back(member);
	}

	// 空になった from グループを削除する
	groups_.erase(fromId);
}

void StageBlockField::RemoveBlockFromGroup(Block* block){
	if(block == nullptr){
		return;
	}

	int groupId = block->GetGroupId();
	if(groupId == kInvalidGroupId){
		return;
	}

	auto it = groups_.find(groupId);
	if(it == groups_.end()){
		block->SetGroupId(kInvalidGroupId);
		block->SetGroupIndex(-1);
		return;
	}

	std::vector<Block*>& members = it->second;
	int index = block->GetGroupIndex();

	if(index >= 0 && index < static_cast<int>(members.size()) && members[index] == block){
		// swap-and-pop で O(1) 削除
		Block* last = members.back();
		members[index] = last;
		if(last != block){
			last->SetGroupIndex(index);
		}
		members.pop_back();
	} else{
		// 異常系フォールバック: groupIndex_ が実際の位置とズレている場合、
		// 線形探索してでも確実に取り除く（残すとダングリングポインタの原因になる）
		auto found = std::find(members.begin(),members.end(),block);
		if(found != members.end()){
			size_t foundIndex = static_cast<size_t>(found - members.begin());
			Block* last = members.back();
			members[foundIndex] = last;
			if(last != block){
				last->SetGroupIndex(static_cast<int>(foundIndex));
			}
			members.pop_back();
		}
	}

	if(members.empty()){
		groups_.erase(it);
	}

	block->SetGroupId(kInvalidGroupId);
	block->SetGroupIndex(-1);
}

void StageBlockField::RemoveGroup(int groupId){
	auto it = groups_.find(groupId);
	if(it == groups_.end()){
		return;
	}

	// 削除中に更新すると古い状態を見てしまうため、先に消えるマスの座標を集めておき、
	// 実際の削除が終わった後でその4近傍のオートタイルを更新する。
	std::vector<GridPos> removedPositions;
	removedPositions.reserve(it->second.size());
	for(Block* member : it->second){
		if(member == nullptr){
			continue;
		}
		removedPositions.push_back(member->GetGridPos());
	}

	// グループ内の全ブロックを cells_ から取り除く（GameObject の破棄は呼び出し側の責務）
	for(Block* member : it->second){
		if(member == nullptr){
			continue;
		}
		EraseCellIfOwnedBy(member);
		member->SetGroupId(kInvalidGroupId);
		member->SetGroupIndex(-1);
	}

	groups_.erase(it);

	// 消えたマスの4近傍に残っているブロックの見た目を更新する
	for(const GridPos& pos : removedPositions){
		RefreshTileModelAround(pos);
	}
}

void StageBlockField::RemoveBlockFromField(Block* block){
	if(block == nullptr){
		return;
	}

	// 削除前に、消えるマスの座標を控えておく（削除後は GetGridPos が古い値のままでも困らないが、
	// 他の削除処理と書き方を揃えるためここで確定させる）
	const GridPos removedPos = block->GetGridPos();

	// 分割(split)検査は行わない（呼び出し元が実害の無い状況で使うことを想定）
	RemoveBlockFromGroup(block);
	EraseCellIfOwnedBy(block);

	// 消えたマスの4近傍に残っているブロックの見た目を更新する
	RefreshTileModelAround(removedPos);
}

void StageBlockField::EraseCellIfOwnedBy(Block* block){
	if(block == nullptr){
		return;
	}
	auto it = cells_.find(block->GetGridPos());
	if(it != cells_.end() && it->second == block){
		cells_.erase(it);
	}
}

const std::vector<Block*>* StageBlockField::GetGroup(int groupId) const{
	auto it = groups_.find(groupId);
	if(it == groups_.end()){
		return nullptr;
	}
	return &it->second;
}

Block* StageBlockField::GetBlockAt(const GridPos& pos) const{
	auto it = cells_.find(pos);
	if(it == cells_.end()){
		return nullptr;
	}
	return it->second;
}

bool StageBlockField::HasSpaceAbove(const GridPos& pos,int cellCount) const{
	// 真上から順に、求められたマス数だけ空いているかを見る
	for(int i = 1; i <= cellCount; ++i){
		const GridPos upper{pos.x,pos.y + i};

		// Wallは連結の対象外だが、そこにプレイヤーは入れないので埋まっている扱いにする
		if(cells_.find(upper) != cells_.end()){
			return false;
		}
		if(wallCells_.find(upper) != wallCells_.end()){
			return false;
		}
	}

	return true;
}

void StageBlockField::SetBlockCollisionCallBacks(PlayerBlockCollisionCallBacks* callBacks){
	pBlockCallBacks_ = callBacks;

	// 着地判定は「ブロックの真上にプレイヤーが入れる空きがあるか」をグリッドで確かめる。
	// そのためにコールバック側へこのクラスを渡しておく
	if(callBacks != nullptr){
		callBacks->SetBlockField(this);
	}
}

void StageBlockField::Clear(){
	// 生成済みの全 Block / Wall を破棄してから、セル・グループ・段の表を消去する
	for(auto& pair : segments_){
		DestroySegmentContent(pair.second);
	}
	segments_.clear();

	// 段から切り離されたブロック（集合・打ち上げの対象になったもの）も破棄する。
	// 連結グループ表からは切り離した時点で既に外れているため、解決表からの登録解除と破棄だけでよい。
	for(std::unique_ptr<Block>& block : detachedBlocks_){
		if(block == nullptr){
			continue;
		}
		if(pBlockCallBacks_ != nullptr){
			pBlockCallBacks_->UnregisterBlock(block.get());
		}
		block->Destroy();
	}
	detachedBlocks_.clear();

	cells_.clear();
	wallCells_.clear();
	groups_.clear();
	nextGroupId_ = 0;
}

Math::Vector3 StageBlockField::GridToWorld(const GridPos& pos){
	constexpr float kBlockSize = 1.0f;
	constexpr float kOffsetX = (kBlockCol - 1) * 0.5f;
	constexpr float kHalfBlockSize = kBlockSize * 0.5f;

	// Xはセグメントの中央が原点に来るように寄せる
	float worldX = (static_cast<float>(pos.x) - kOffsetX) * kBlockSize;
	// Yは一番下の段(y = 0)の下面が高さ0になるように積む(その段の中心は 0.5)
	float worldY = static_cast<float>(pos.y) * kBlockSize + kHalfBlockSize;
	float worldZ = 0.0f;

	return Math::Vector3(worldX,worldY,worldZ);
}

GridPos StageBlockField::WorldToGrid(const Math::Vector3& worldPos){
	constexpr float kBlockSize = 1.0f;
	constexpr float kOffsetX = (kBlockCol - 1) * 0.5f;

	// GridToWorldの逆変換。Xはブロック中心が世界座標そのもの、Yは下面基準なのでfloorで求める。
	int gridX = static_cast<int>(std::floor(worldPos.x / kBlockSize + kOffsetX + 0.5f));
	int gridY = static_cast<int>(std::floor(worldPos.y / kBlockSize));

	return GridPos{gridX,gridY};
}

std::vector<Block*> StageBlockField::GetBlocksInWorldAABB(const Math::Vector3& worldMin,const Math::Vector3& worldMax) const{
	std::vector<Block*> result;

	const GridPos minPos = WorldToGrid(worldMin);
	const GridPos maxPos = WorldToGrid(worldMax);

	for(int x = minPos.x; x <= maxPos.x; ++x){
		for(int y = minPos.y; y <= maxPos.y; ++y){
			if(Block* block = GetBlockAt(GridPos{x,y})){
				result.push_back(block);
			}
		}
	}

	return result;
}

std::vector<Wall*> StageBlockField::GetWallsInWorldAABB(const Math::Vector3& worldMin,const Math::Vector3& worldMax) const{
	std::vector<Wall*> result;

	const GridPos minPos = WorldToGrid(worldMin);
	const GridPos maxPos = WorldToGrid(worldMax);

	for(int x = minPos.x; x <= maxPos.x; ++x){
		for(int y = minPos.y; y <= maxPos.y; ++y){
			auto it = wallCells_.find(GridPos{x,y});
			if(it != wallCells_.end() && it->second != nullptr){
				result.push_back(it->second);
			}
		}
	}

	return result;
}

std::vector<Block*> StageBlockField::GetLandableBlocks() const{
	std::vector<Block*> result;

	for(const auto& cell : cells_){

		// continue
		Block* block = cell.second;
		if(block == nullptr || !block->IsValid()){
			continue;
		}

		// 真上に別のブロックが積まれていたら、そこには乗れないので候補から外す
		const GridPos upper{cell.first.x,cell.first.y + 1};
		if(cells_.find(upper) != cells_.end()){
			continue;
		}

		// 落とす対象として候補追加
		result.push_back(block);
	}

	return result;
}

bool StageBlockField::TryGetGroupCenter(int groupId,Math::Vector3& outCenter) const{
	// そのIDのグループが無ければ中心も出せない
	auto it = groups_.find(groupId);
	if(it == groups_.end()){
		return false;
	}

	Math::Vector3 sum = CVector3::ZERO;
	int validCount = 0;

	// メンバーの座標を足し込みながら、実際に足せた個数を数える
	for(const Block* member : it->second){

		// 既にWorldから消えているブロックは中心の計算に入れない
		if(member == nullptr || !member->IsValid()){
			continue;
		}

		// 座標はTransformから引く。取れないものも同じく飛ばす
		const AOENGINE::WorldTransform* transform = member->GetTransform();
		if(transform == nullptr){
			continue;
		}

		// 合計に足して、数えた個数を進める
		sum = sum + transform->GetTranslate();
		++validCount;
	}

	// 1個も足せていない場合、この後の割り算がゼロ除算になるので失敗を返す
	if(validCount == 0){
		return false;
	}

	// 平均を求めて返す
	outCenter = sum * (1.0f / static_cast<float>(validCount));
	return true;
}

void StageBlockField::SetGroupColor(int groupId,const AOENGINE::Color& color){
	auto it = groups_.find(groupId);
	if(it == groups_.end()){
		return;
	}

	for(Block* member : it->second){
		if(member == nullptr){
			continue;
		}

		AOENGINE::BaseGameObject* gameObject = member->GetGameObject();
		if(gameObject == nullptr){
			continue;
		}

		gameObject->SetColor(color);
	}
}

#ifndef NDEBUG
void StageBlockField::ApplyDebugGroupColors(){
	// デバッグ用: グループごとに異なる色を設定し、グルーピングが意図通りに
	// 効いているかを目視で確認できるようにする。
	for(const auto& group : groups_){
		SetGroupColor(group.first,MakeDebugGroupColor(group.first));
	}
}
#endif

AOENGINE::Color StageBlockField::MakeDebugGroupColor(int groupId){
	if(groupId == kInvalidGroupId){
		return AOENGINE::Color(0.5f,0.5f,0.5f,1.0f);
	}

	// 黄金比で色相を回すことで、隣り合うIDでも色が離れて見分けやすくなる
	constexpr float kGoldenRatioConjugate = 0.618033988749895f;
	float hue = std::fmodf(static_cast<float>(groupId) * kGoldenRatioConjugate,1.0f);
	constexpr float s = 0.75f;
	constexpr float v = 1.0f;

	// HSV -> RGB（標準的な6セクタの式）
	float h6 = hue * 6.0f;
	int sector = static_cast<int>(h6) % 6;
	float f = h6 - std::floor(h6);

	float p = v * (1.0f - s);
	float q = v * (1.0f - s * f);
	float t = v * (1.0f - s * (1.0f - f));

	float r = 0.0f,g = 0.0f,b = 0.0f;
	switch(sector){
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		default: r = v; g = p; b = q; break;
	}

	return AOENGINE::Color(r,g,b,1.0f);
}

void StageBlockField::BuildSegment(const StageSegment& data,int segmentIndex){
	if(segments_.find(segmentIndex) != segments_.end()){
		// 既に同じ segmentIndex が生成済みの場合は何もしない（二重生成の防止）
		return;
	}

	SegmentContent& content = segments_[segmentIndex];

	// このセグメントで新規生成した座標を集めておき、生成完了後にまとめてオートタイルを更新する
	std::vector<GridPos> builtPositions;

	for(int i = 0; i < kBlockRow; ++i){
		int y = ((kBlockRow - 1) - i) + segmentIndex * kBlockRow; // CSVは上の行ほど画面上側を表すため、行インデックスを反転させる
		for(int j = 0; j < kBlockCol; ++j){
			const int cell = data.GetCell(i,j);
			if(cell != kBlockCell && cell != kWallCell){
				continue;
			}

			// セグメント i行 j列 -> グローバルグリッド座標へ変換
			GridPos pos{};
			pos.x = j;
			pos.y = y;

			if(cell == kWallCell){
				CreateWall(content,pos);
			} else{
				CreateBlock(content,pos);
			}

			builtPositions.push_back(pos);
		}
	}

	// 新規生成したマスとその4近傍にある既存ブロック(1つ下の段の最上行など)の見た目を更新する
	for(const GridPos& pos : builtPositions){
		RefreshTileModelAround(pos);
	}
}

void StageBlockField::CreateBlock(SegmentContent& content,const GridPos& pos){
	// プレハブから Block の実体となる GameObject を生成する
	AOENGINE::BaseGameObject* gameObject = InstantiateStageObject("Block");
	if(gameObject == nullptr){
		return;
	}

	// Block を生成し、生成した GameObject と関連付ける（所有権はこのクラスが持つ）
	std::unique_ptr<Block> block = std::make_unique<Block>();
	block->Bind(gameObject);

	// グリッド座標から求めたワールド座標を設定する
	block->GetTransform()->SetTranslate(GridToWorld(pos));

	// 連結グループ表に登録する（4近傍との連結もここで解決される）
	AddBlock(block.get(),pos);

	// 衝突したColliderから着地したBlockを引けるようにする
	if(pBlockCallBacks_ != nullptr){
		pBlockCallBacks_->RegisterBlock(block.get());
	}

	content.blocks.push_back(std::move(block));
}

void StageBlockField::CreateWall(SegmentContent& content,const GridPos& pos){
	// プレハブから Wall の実体となる GameObject を生成する
	AOENGINE::BaseGameObject* gameObject = InstantiateStageObject("Wall");
	if(gameObject == nullptr){
		return;
	}

	std::unique_ptr<Wall> wall = std::make_unique<Wall>();
	wall->Bind(gameObject);
	wall->SetGridPos(pos);

	// 足場としての位置はBlockと同じ座標系で求める
	wall->GetTransform()->SetTranslate(GridToWorld(pos));

	// Wall は連結・打ち上げの対象にしないため連結グループ表には登録しない。
	// ただし足場としては乗れるので、グリッド表にだけ入れておく
	wallCells_[pos] = wall.get();

	content.walls.push_back(std::move(wall));
}

void StageBlockField::DestroySegmentContent(SegmentContent& content){
	// 削除中に更新すると古い状態を見てしまうため、先に消えるマス(Block/Wallの両方)の座標を
	// 集めておき、実際の削除が終わった後でその4近傍のオートタイルを更新する。
	std::vector<GridPos> removedPositions;
	removedPositions.reserve(content.blocks.size() + content.walls.size());
	for(const std::unique_ptr<Block>& block : content.blocks){
		if(block != nullptr){
			removedPositions.push_back(block->GetGridPos());
		}
	}
	for(const std::unique_ptr<Wall>& wall : content.walls){
		if(wall != nullptr){
			removedPositions.push_back(wall->GetGridPos());
		}
	}

	// これから消える予定のブロックに対して RefreshTileModelAround() 経由の無駄な
	// モデル差し替え(SetObject)が走らないよう、破棄ループの間だけ個別更新を止める。
	isBulkEditing_ = true;

	// このセグメントが所有する Block を連結グループ表から外し、GameObject を破棄する。
	// セグメント破棄は連結グループの一部だけを消すことになるが、
	// 画面外（ストリーミングで既に見えなくなった範囲）でのみ行われるため、
	// 残りの連結性についての分割(split)検査はここでは行わない。
	for(std::unique_ptr<Block>& block : content.blocks){
		if(block == nullptr){
			continue;
		}
		if(pBlockCallBacks_ != nullptr){
			pBlockCallBacks_->UnregisterBlock(block.get());
		}
		RemoveBlockFromField(block.get());
		block->Destroy();
	}
	content.blocks.clear();

	// Wall は連結グループ表に登録していないため、グリッド表から外して破棄するだけでよい
	for(std::unique_ptr<Wall>& wall : content.walls){
		if(wall == nullptr){
			continue;
		}
		// 別の Wall が既に同じ座標を占有している場合に、それを消してしまわないように確認する
		auto it = wallCells_.find(wall->GetGridPos());
		if(it != wallCells_.end() && it->second == wall.get()){
			wallCells_.erase(it);
		}
		wall->Destroy();
	}
	content.walls.clear();

	// 破棄がすべて終わったので、ここから先はまとめての最終更新だけを行う。
	// 戻し忘れると以降オートタイルの見た目更新が一切効かなくなるため注意。
	isBulkEditing_ = false;

	// 消えたマスの4近傍に残っているブロックの見た目を更新する
	for(const GridPos& pos : removedPositions){
		RefreshTileModelAround(pos);
	}
}

void StageBlockField::DestroySegment(int segmentIndex){
	auto it = segments_.find(segmentIndex);
	if(it == segments_.end()){
		return;
	}

	DestroySegmentContent(it->second);
	segments_.erase(it);
}

void StageBlockField::DetachGroup(int groupId){
	auto it = groups_.find(groupId);
	if(it == groups_.end()){
		return;
	}

	// RemoveGroup() でメンバー配列が無効になるため、先に実体をコピーしておく
	const std::vector<Block*> members = it->second;

	for(Block* member : members){
		DetachBlock(member);
	}

	// 連結グループ表（cells_ / groups_）からも外す
	RemoveGroup(groupId);
}

void StageBlockField::DestroyDetachedBlock(Block* block){
	if(block == nullptr){
		return;
	}

	// 切り離し済みのブロックだけが対象。段が持っているブロックは段の破棄で消えるため、ここでは触らない
	auto found = std::find_if(detachedBlocks_.begin(),detachedBlocks_.end(),
							  [block](const std::unique_ptr<Block>& owned){ return owned.get() == block; });
	if(found == detachedBlocks_.end()){
		return;
	}

	// 連結グループ表からは DetachGroup() の時点で既に外れているため、
	// 解決表からの登録解除と GameObject の破棄だけでよい
	if(pBlockCallBacks_ != nullptr){
		pBlockCallBacks_->UnregisterBlock(block);
	}
	block->Destroy();
	detachedBlocks_.erase(found);
}

void StageBlockField::DetachBlock(Block* block){
	if(block == nullptr){
		return;
	}

	// 段は数個、1段のブロック数も有界で、呼ばれるのも集合開始時だけなので線形探索で十分
	for(auto& pair : segments_){
		std::vector<std::unique_ptr<Block>>& blocks = pair.second.blocks;

		auto found = std::find_if(blocks.begin(),blocks.end(),
								  [block](const std::unique_ptr<Block>& owned){ return owned.get() == block; });
		if(found == blocks.end()){
			continue;
		}

		detachedBlocks_.push_back(std::move(*found));
		blocks.erase(found);
		return;
	}
}

bool StageBlockField::IsCellOccupied(const GridPos& pos) const{
	// Blockがあれば埋まっている
	if(cells_.find(pos) != cells_.end()){
		return true;
	}

	// kTreatWallAsConnected が true の間は Wall も埋まっている扱いにする
	if(kTreatWallAsConnected && wallCells_.find(pos) != wallCells_.end()){
		return true;
	}

	return false;
}

uint8_t StageBlockField::CalcNeighborMask(const GridPos& pos) const{
	uint8_t mask = 0;

	if(IsCellOccupied(GridPos{pos.x,pos.y + 1})){
		mask |= kBlockNeighborUp;
	}
	if(IsCellOccupied(GridPos{pos.x,pos.y - 1})){
		mask |= kBlockNeighborDown;
	}
	if(IsCellOccupied(GridPos{pos.x - 1,pos.y})){
		mask |= kBlockNeighborLeft;
	}
	if(IsCellOccupied(GridPos{pos.x + 1,pos.y})){
		mask |= kBlockNeighborRight;
	}

	return mask;
}

void StageBlockField::RefreshTileModel(Block* block){
	if(block == nullptr || !block->IsValid()){
		return;
	}

	if(isBulkEditing_){
		return;
	}

	// 段から切り離された(集合・打ち上げ中の)ブロックは今回のオートタイルの対象外。
	// 見た目はその時点のまま固定でよいため、cells_ に自分がいる場合のみ更新する。
	auto cellIt = cells_.find(block->GetGridPos());
	if(cellIt == cells_.end() || cellIt->second != block){
		return;
	}

	uint8_t mask = CalcNeighborMask(block->GetGridPos());
	if(static_cast<int>(mask) == block->GetTileMask()){
		// 前回と同じ隣接パターンならモデル差し替えは丸ごとスキップする(SetObjectは重いため)
		return;
	}

	const BlockTileAppearance& appearance = BlockAutoTile::GetAppearance(mask);

	AOENGINE::BaseGameObject* gameObject = block->GetGameObject();
	if(gameObject == nullptr){
		return;
	}

	// SetObject() はマテリアルを作り直して色を消してしまうため、先に現在の色を退避しておく。
	// マテリアルが取れない場合はBlock prefab既定色にフォールバックする。
	AOENGINE::Color color = kDefaultBlockColor;
	if(AOENGINE::BaseMaterial* material = gameObject->GetMaterial(0)){
		color = material->GetColor();
	}

	gameObject->SetObject(appearance.modelName);
	gameObject->SetColor(color);

	// オートタイル用モデルは2×2×2サイズなので、1マスに合わせて縮小する
	block->GetTransform()->SetScale(Math::Vector3(kTileModelScale,kTileModelScale,kTileModelScale));

	// BoxCollider::Update() は transform の scale をAABBに掛けるため、
	// 縮小した分だけコライダーを大きくして当たり判定を1マスに保つ
	if(auto* boxCollider = dynamic_cast<AOENGINE::BoxCollider*>(block->GetCollider("Block"))){
		boxCollider->SetSize(Math::Vector3(kTileColliderSize,kTileColliderSize,kTileColliderSize));
	}

	// Z軸回転で、モデル空間の露出方向を世界の露出方向へ合わせる。
	// 回転の向きが逆に見える場合は kTileRotateSign を -1.0f にすれば直る。
	block->GetTransform()->SetRotate(
		Math::Quaternion::AngleAxis(appearance.rotateZDegree * kTileRotateSign * kToRadian,Math::Vector3(0.0f,0.0f,1.0f)));

	block->SetTileMask(static_cast<int>(mask));
}

void StageBlockField::RefreshTileModelAround(const GridPos& pos){
	// 自分自身
	if(Block* self = GetBlockAt(pos)){
		RefreshTileModel(self);
	}

	// 4近傍
	for(const GridPos& offset : kNeighborOffsets){
		GridPos neighborPos{pos.x + offset.x,pos.y + offset.y};
		if(Block* neighbor = GetBlockAt(neighborPos)){
			RefreshTileModel(neighbor);
		}
	}
}

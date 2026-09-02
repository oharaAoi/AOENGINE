#include "GameScene.h"

#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/System/Manager/PrefabManager.h"

/// game
#include "Game/Actor/Player/Player.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/WorldObject/Block.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	Finalize();
}

void GameScene::Finalize(){
	// ステージのブロックを SceneWorld から破棄し、連結グループ表を空にする。
	// GameScene のデストラクタからも呼ばれるため、複数回呼ばれても安全であること。
	ClearStage();
	player_.reset();
}

void GameScene::Init(){
	AOENGINE::Render::GetLightGroup()->Load();

	player_ = std::make_unique<Player>();

	// 着地したブロックのグループをPlayerへ渡すコールバックを衝突ペアへ登録する
	playerBlockCallBacks_.SetPlayer(player_.get());
	playerBlockCallBacks_.Init();
	playerBlockCallBacks_.SetPair(collisionManager_.get(),"Player","Block");
}

void GameScene::OnPlayStart(){
	// Playを押し直すと再度呼ばれるため、前回の生成物を片付けてから作り直す
	ClearStage();
	SetupStage();

	// Player初期化
	player_->Init(ResolvePlayerBody());
	// 接続したグループを集合・打ち上げさせるために連結グループ表を渡す
	player_->SetBlockField(&stageBlockField_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update(){

	// プレイヤー
	if(player_){
		player_->Update();
	}

	// フォローカメラ
	if(followCamera_){
		followCamera_->Update();
	}

#ifdef _DEVELOPMENT
	// 調整パラメータの編集 + Save/Load
	if(player_){
		ImGui::Begin("Player");
		player_->Debug_Gui();
		ImGui::End();
	}
	if(followCamera_){
		ImGui::Begin("FollowCamera");
		followCamera_->Debug_Gui();
		ImGui::End();
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Draw() const{
	if(!followCamera_ || !followCamera_->HasTarget()){
		// Playerが居ないときは通常のCamera3dで描画する
		BaseScene::Draw();
		return;
	}

	// BaseScene::Draw()のGameCameraをFollowCameraに差し替えたもの
	if(pSceneRenderer_){
		pSceneRenderer_->DrawShadowMap();

		// FollowCamera から描画する
		Engine::BeginSceneView(SceneViewType::Game);
		followCamera_->ApplyToRender();
		Engine::CommitSceneViewCamera(SceneViewType::Game);
		skybox_->Draw();
		pSceneRenderer_->DrawSceneObjects(
			followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix());

	#ifdef _DEVELOPMENT

		Engine::BeginSceneView(SceneViewType::Editor);
		debugCamera_->ApplyToRender();
		Engine::CommitSceneViewCamera(SceneViewType::Editor);
		skybox_->Draw();
		pSceneRenderer_->DrawSceneObjects(
			debugCamera_->GetViewMatrix() * debugCamera_->GetProjectionMatrix());
	#endif
	}

	// PostProcess/PostDrawはGame View に対して実行される
	Engine::ActivateSceneView(SceneViewType::Game);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ステージの生成・片付け
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::SetupStage(){
	stageSegment_.LoadBlockData("./Project/Assets/Game/StageData/test.csv");
	stageSegment_.SetupSegmentOnWorld(&stageBlockField_,0);

	// 衝突したColliderから着地したBlockを引けるようにする
	for(const std::unique_ptr<Block>& block : stageSegment_.GetBlocks()){
		playerBlockCallBacks_.RegisterBlock(block.get());
	}
}

void GameScene::ClearStage(){
	playerBlockCallBacks_.ClearBlocks();
	stageSegment_.UnregisterFromWorld(&stageBlockField_);
	stageBlockField_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Playerの本体の用意
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::BaseGameObject* GameScene::ResolvePlayerBody(){
	// Sceneに置かれているPlayerを優先して使う
	if(AOENGINE::BaseGameObject* body = FindSceneObject<AOENGINE::BaseGameObject>("Player")){
		return body;
	}

	// 無ければPrefabから生成する
	AOENGINE::SceneObject* root = AOENGINE::PrefabManager::GetInstance()->Instantiate("Player");
	return dynamic_cast<AOENGINE::BaseGameObject*>(root);
}



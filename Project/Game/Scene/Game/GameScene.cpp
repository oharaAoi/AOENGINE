#include "GameScene.h"

#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/System/Manager/PrefabManager.h"

/// game
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Floor/DamageFloor.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/WorldObject/Block.h"

GameScene::GameScene() {}

GameScene::~GameScene()
{
	Finalize();
}

void GameScene::Finalize()
{
	boss_.reset();
	damageFloor_.reset();
	followCamera_.reset();
	player_.reset();

	// ステージのブロックを SceneWorld から破棄し、連結グループ表を空にする。
	// GameScene のデストラクタからも呼ばれるため、複数回呼ばれても安全であること。
	ClearStage();
	player_.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理(インスタンスの宣言など)
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init()
{
	AOENGINE::Render::GetLightGroup()->Load();

	player_ = std::make_unique<Player>();
	followCamera_ = std::make_unique<FollowCamera>();

	boss_ = std::make_unique<Boss>();

	// 背景
	backgrounds_ = std::make_unique<StageBackgrounds>();
	// ダメージ床
	damageFloor_ = std::make_unique<DamageFloor>();

	// 着地したブロックのグループをPlayerへ渡すコールバックを衝突ペアへ登録する
	playerBlockCallBacks_.SetPlayer(player_.get());
	playerBlockCallBacks_.Init();
	playerBlockCallBacks_.SetPair(collisionManager_.get(), "Player", "Block");

	// コールバック関数の作成
	callBacks_.Init(collisionManager_.get(), player_.get(), damageFloor_.get());
}

void GameScene::OnPlayStart()
{

	/*backgrounds_->Init();*/
	// プレイヤー初期化
	player_->Init(ResolvePlayerBody());
	// ボス初期化
	boss_->Init(FindSceneObject<AOENGINE::BaseGameObject>("Boss"));
	// ダメージ床の初期化
	damageFloor_->Init(FindSceneObject<AOENGINE::BaseGameObject>("DamageFloor"));
	// カメラ初期化
	followCamera_->Init();
	// 背景の初期化
	backgrounds_->Init(&stageBlockField_, &stageSegment_);

	ClearStage();
	SetupStage();

	// Player初期化
	// 接続したグループを集合・打ち上げさせるために連結グループ表を渡す
	player_->SetBlockField(&stageBlockField_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update()
{

	// プレイヤー
	if (player_)
	{
		player_->Update();
	}

	// フォローカメラ
	if (followCamera_)
	{
		// ダメージ床で打ち上げられてから着地するまでは、そのままプレイヤーへ常に追従する
		followCamera_->SetContinuousFollow(player_ && player_->IsDamageFloorAirborne());
		followCamera_->Update();
	}

	// 背景
	if (backgrounds_) {
		backgrounds_->Update(&stageBlockField_, &stageSegment_, player_->GetPosition());
	}

	// ボスの更新
	if (boss_ && followCamera_)
	{
		const Math::Matrix4x4 viewProjection =
			followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix();
		boss_->Update(viewProjection);
	}

	if (damageFloor_ && followCamera_) {
		const Math::Matrix4x4 viewProjection =
			followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix();
		damageFloor_->Update(viewProjection);
	}

#ifdef _DEVELOPMENT
	// 調整パラメータの編集
	ImGui::Begin("GameScene Parameters");

	if (player_ && ImGui::CollapsingHeader("Player"))
	{
		ImGui::PushID("Player");
		player_->Debug_Gui();
		ImGui::PopID();
	}
	if (followCamera_ && ImGui::CollapsingHeader("FollowCamera"))
	{
		ImGui::PushID("FollowCamera");
		followCamera_->Debug_Gui();
		ImGui::PopID();
	}
	if (boss_ && ImGui::CollapsingHeader("Boss"))
	{
		ImGui::PushID("Boss");
		boss_->Debug_Gui();
		ImGui::PopID();
	}

	ImGui::End();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Draw() const
{
	if (!followCamera_ || !followCamera_->HasTarget())
	{
		// Playerが居ないときは通常のCamera3dで描画する
		BaseScene::Draw();
		return;
	}

	// BaseScene::Draw()のGameCameraをFollowCameraに差し替えたもの
	if (pSceneRenderer_)
	{
		pSceneRenderer_->DrawShadowMap();

		// FollowCamera から描画する
		Engine::BeginSceneView(SceneViewType::Game);
		followCamera_->ApplyToRender();
		Engine::CommitSceneViewCamera(SceneViewType::Game);
		skybox_->Draw();
		Engine::GetCanvas2d()->Update();
		Engine::DrawBackgroundSprites(SceneViewType::Game);
		// 背景Sprite用の単一RTから3D用MRTへ、内容を保持したまま戻す。
		Engine::BeginSceneView(SceneViewType::Game, false);
		pSceneRenderer_->DrawSceneObjects(
			followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix());

#ifdef _DEVELOPMENT

		Engine::BeginSceneView(SceneViewType::Editor);
		debugCamera_->ApplyToRender();
		Engine::CommitSceneViewCamera(SceneViewType::Editor);
		skybox_->Draw();
		Engine::DrawBackgroundSprites(SceneViewType::Editor);
		Engine::BeginSceneView(SceneViewType::Editor, false);
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

void GameScene::SetupStage()
{
	stageSegment_.LoadBlockData("./Project/Assets/Game/StageData/test.csv");
	stageSegment_.SetupSegmentOnWorld(&stageBlockField_, 0);

	// 衝突したColliderから着地したBlockを引けるようにする
	for (const std::unique_ptr<Block> &block : stageSegment_.GetBlocks())
	{
		playerBlockCallBacks_.RegisterBlock(block.get());
	}
}

void GameScene::ClearStage()
{
	playerBlockCallBacks_.ClearBlocks();
	stageSegment_.UnregisterFromWorld(&stageBlockField_);
	stageBlockField_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Playerの本体の用意
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::BaseGameObject *GameScene::ResolvePlayerBody()
{
	// Sceneに置かれているPlayerを優先して使う
	if (AOENGINE::BaseGameObject *body = FindSceneObject<AOENGINE::BaseGameObject>("Player"))
	{
		return body;
	}

	// 無ければPrefabから生成する
	AOENGINE::SceneObject *root = AOENGINE::PrefabManager::GetInstance()->Instantiate("Player");
	return dynamic_cast<AOENGINE::BaseGameObject *>(root);
}

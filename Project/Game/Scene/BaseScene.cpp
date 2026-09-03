#include "BaseScene.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Module/PostEffect/PostProcess.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"

BaseScene::~BaseScene() {
	skybox_ = nullptr;
	// SceneRendererは全シーンで共有するSingletonのため、Sceneの破棄では終了しない。
	// シーン切り替え時の内容クリアは、次のSceneRenderer::Init()が担当する。
	pSceneRenderer_ = nullptr;
	if (collisionManager_) {
		collisionManager_->Finalize();
	}
}

void BaseScene::Initialize() {
	skybox_ = std::make_unique<Skybox>();
	skybox_->Init();

	// -------------------------------------------------
	// ↓ Rendererの初期化
	// -------------------------------------------------
	pSceneRenderer_ = AOENGINE::SceneRenderer::GetInstance();
	pSceneRenderer_->Init();

	Engine::GetCanvas2d()->Init();

	// -------------------------------------------------
	// ↓ Collision関連の初期化
	// -------------------------------------------------
	collisionManager_ = std::make_unique<AOENGINE::CollisionManager>();
	collisionManager_->Init();

	// -------------------------------------------------
	// ↓ cameraの初期化
	// -------------------------------------------------
	camera2d_ = std::make_unique<Camera2d>();
	camera3d_ = std::make_unique<Camera3d>();
	debugCamera_ = std::make_unique<DebugCamera>();

	camera2d_->Init();
	camera3d_->Init();
	debugCamera_->Init();
	RegisterCamera();

	// -------------------------------------------------
	// ↓ 外部保存パラメータの初期化
	// -------------------------------------------------
	AOENGINE::JsonItems* adjust = AOENGINE::JsonItems::GetInstance();
	adjust->Init();

	// Renderer、Camera、Collisionなどの共通状態が揃ってからシーン固有初期化を行う。
	Init();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ sceneの読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::LoadScene(const std::string& directory, const std::string& fileName, const std::string& extension) {
	collisionManager_->Init();
	
	pSceneLoader_ = AOENGINE::SceneLoader::GetInstance();
	pSceneLoader_->Init();
	pSceneLoader_->Load(directory, fileName, extension);

	pSceneRenderer_ = AOENGINE::SceneRenderer::GetInstance();
	pSceneRenderer_->Init();
	pSceneRenderer_->CreateObject(pSceneLoader_->GetLevelData());
	AOENGINE::EditorWindows::GetInstance()->SetSceneRenderer(pSceneRenderer_);

	camera2d_->Init();
	camera3d_->Init();
	debugCamera_->Init();

	RegisterCamera();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Updateより後に呼び出される更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::UpdateProcess() {
	this->Update();
	skybox_->Update();
	pSceneRenderer_->Update();
	collisionManager_->CheckHorizontalCollision();
	pSceneRenderer_->ApplyCollisionPushback();
	pSceneRenderer_->UpdateVerticalPhysics();
	collisionManager_->CheckAllCollision();

	camera3d_->Update();
	debugCamera_->Update();
	camera2d_->Update();

	pSceneRenderer_->PostUpdate();
	this->PostUpdate();
}

void BaseScene::EditorUpdateProcess() {
	// ゲームロジック、Physics、Animation時間は進めない。
	pSceneRenderer_->EditorUpdate();
	skybox_->Update();

	// Scene Viewの操作と2D描画行列はEdit/Pause中も必要。
	debugCamera_->Update();
	camera3d_->Update();
	camera2d_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::Draw() const {
	if (pSceneRenderer_) {
		// Shadow MapはCameraに依存しないため、両Viewで共有して1度だけ生成する。
		pSceneRenderer_->DrawShadowMap();

		// Game View: 配置されたCamera3dから描画する。
		Engine::BeginSceneView(SceneViewType::Game);
		camera3d_->ApplyToRender();
		Engine::CommitSceneViewCamera(SceneViewType::Game);
		skybox_->Draw();
		Engine::GetCanvas2d()->Update();
		Engine::DrawBackgroundSprites(SceneViewType::Game);
		// 背景Sprite用の単一RTから3D用MRTへ戻す。
		Engine::BeginSceneView(SceneViewType::Game, false);
		pSceneRenderer_->DrawSceneObjects(
			camera3d_->GetViewMatrix() * camera3d_->GetProjectionMatrix());

#ifdef _DEVELOPMENT
		// Scene View: DebugCameraから同じSceneWorldを別RenderTargetへ描画する。
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

	// PostProcess/PostDrawはGame Viewに対して実行されるため、Game Cameraへ戻しておく。
	Engine::ActivateSceneView(SceneViewType::Game);
}

void BaseScene::RegisterCamera() {
	auto& world = pSceneRenderer_->GetSceneWorld();

	world.AddExternalObject(*camera3d_, "Camera3D");
	world.AddExternalObject(*debugCamera_, "DebugCamera");
	world.AddExternalObject(*camera2d_, "Camera2D");
}

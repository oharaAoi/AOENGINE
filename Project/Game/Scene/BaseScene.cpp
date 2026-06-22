#include "BaseScene.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Module/PostEffect/PostProcess.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"

BaseScene::~BaseScene() {
	pSceneRenderer_->Finalize();
	collisionManager_->Finalize();
}

void BaseScene::Init() {
	pSceneRenderer_ = AOENGINE::SceneRenderer::GetInstance();
	pSceneRenderer_->Init();

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

	skybox_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<Skybox>("Skybox", "Object_Skybox.json", -999);
	AOENGINE::Render::SetSkyboxTexture(skybox_->GetTexture());

	camera2d_->Init();
	camera3d_->Init();
	debugCamera_->Init();

	RegisterCamera();

	AOENGINE::EditorWindows::GetInstance()->AddObjectWindow(pSceneRenderer_, "sceneRenderer");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Updateより後に呼び出される更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::UpdateProcess() {
	this->Update();
	pSceneRenderer_->Update();
	collisionManager_->CheckAllCollision();

	camera3d_->Update();
	debugCamera_->Update();
	camera2d_->Update();

	pSceneRenderer_->PostUpdate();
	this->PostUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::Draw() const {
	// Shadow MapはCameraに依存しないため、両Viewで共有して1度だけ生成する。
	pSceneRenderer_->DrawShadowMap();

	// Game View: 配置されたCamera3dから描画する。
	camera3d_->ApplyToRender();
	Engine::BeginSceneView(SceneViewType::Game);
	pSceneRenderer_->DrawSceneObjects();

#ifdef _DEBUG
	// Scene View: DebugCameraから同じSceneWorldを別RenderTargetへ描画する。
	debugCamera_->ApplyToRender();
	Engine::BeginSceneView(SceneViewType::Editor);
	pSceneRenderer_->DrawSceneObjects();
#endif

	// PostProcess/PostDrawはGame Viewに対して実行されるため、Game Cameraへ戻しておく。
	Engine::ActivateSceneView(SceneViewType::Game);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ シーンを保存する
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::SaveSceneEffect() {
	Engine::GetPostProcess()->Save(this->sceneName_ + "/");
	//Engine::GetCanvas2d()->Save(this->sceneName_);
}

void BaseScene::LoadSceneEffect() {
	std::string path = AOENGINE::JsonItems::GetDirectoryPath() + this->sceneName_ + "/";
	AOENGINE::JsonItems::GetInstance()->LoadDesignationPath(path);
	Engine::GetPostProcess()->Load(this->sceneName_ + "/");

	//Engine::GetCanvas2d()->Load(this->sceneName_);
}

void BaseScene::RegisterCamera() {
	auto& world = pSceneRenderer_->GetSceneWorld();

	world.AddExternalObject(*camera3d_, "Camera3D");
	world.AddExternalObject(*debugCamera_, "DebugCamera");
	world.AddExternalObject(*camera2d_, "Camera2D");
}

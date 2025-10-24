#include "BaseScene.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Render.h"

BaseScene::~BaseScene() {
	pSceneRenderer_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ sceneの読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::LoadScene(const std::string& directory, const std::string& fileName, const std::string& extension) {
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Init();
	
	pSceneLoader_ = SceneLoader::GetInstance();
	pSceneLoader_->Init();
	pSceneLoader_->Load(directory, fileName, extension);

	pSceneRenderer_ = SceneRenderer::GetInstance();
	pSceneRenderer_->Init();
	pSceneRenderer_->CreateObject(pSceneLoader_->GetLevelData());
	EditorWindows::GetInstance()->SetSceneRenderer(pSceneRenderer_);

	skybox_ = SceneRenderer::GetInstance()->AddObject<Skybox>("Skybox", "Object_Skybox.json", -999);
	Render::SetSkyboxTexture(skybox_->GetTexture());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Updateより後に呼び出される更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::UpdateProcess() {
	this->Update();
	pSceneRenderer_->Update();
	collisionManager_->CheckAllCollision();

	this->PostUpdate();

	pSceneRenderer_->PostUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::Draw() const {
	pSceneRenderer_->Draw();
}
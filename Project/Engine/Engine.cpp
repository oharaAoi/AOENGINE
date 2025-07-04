#include "Engine.h"
#include "Engine/System/ParticleSystem/Tool/EffectSystem.h"
#include "Engine/Lib/Json//JsonItems.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Utilities/Logger.h"

Engine::Engine() {}

Engine::~Engine() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::Initialize(uint32_t backBufferWidth, int32_t backBufferHeight) {
	kClientWidth_ = backBufferWidth;
	kClientHeight_ = backBufferHeight;

	CoInitializeEx(0, COINIT_MULTITHREADED);

	// -------------------------------------------------
	// ↓ インスタンスの作成
	// -------------------------------------------------
	winApp_ = WinApp::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	input_ = Input::GetInstance();
	render_ = Render::GetInstance();
	effectSystem_ = EffectSystem::GetInstacne();
	editorWindows_ = EditorWindows::GetInstance();

	winApp_->CreateGameWindow();
	
	shaders_ = std::make_unique<Shader>();
	computeShader_ = std::make_unique<ComputeShader>();

	processedSceneFrame_ = std::make_unique<ProcessedSceneFrame>();
	audio_ = std::make_unique<Audio>();

	postProcess_ = std::make_unique<PostProcess>();

	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("Engine");

	// -------------------------------------------------
	// ↓ 各初期化
	// -------------------------------------------------
	shaders_->Init();

	graphicsCxt_ = GraphicsContext::GetInstance();
	graphicsCxt_->Init(winApp_, shaders_.get(), kClientWidth_, kClientHeight_);

	dxDevice_ = graphicsCxt_->GetDevice();
	dxCmdList_ = graphicsCxt_->GetCommandList();
	dxHeap_ = graphicsCxt_->GetDxHeap();
	dxCommon_ = graphicsCxt_->GetDxCommon();

	graphicsPipeline_ = graphicsCxt_->GetGraphicsPipeline();
	primitivePipeline_ = graphicsCxt_->GetPrimitivePipeline();

	renderTarget_ = graphicsCxt_->GetRenderTarget();

	textureManager_->Init(dxDevice_, dxCmdList_, dxHeap_);
	computeShader_->Init(dxDevice_, graphicsCxt_->GetDxCompiler(), dxHeap_, graphicsCxt_->GetRenderTarget()->GetRenderTargetSRVHandle(RenderTargetType::Object3D_RenderTarget), shaders_.get());
	input_->Init(winApp_->GetWNDCLASS(), winApp_->GetHwnd());
	processedSceneFrame_->Init(dxDevice_, dxHeap_);


#ifdef _DEBUG
	editorWindows_->Init(dxDevice_, dxCmdList_, renderTarget_, dxHeap_);
	editorWindows_->SetProcessedSceneFrame(processedSceneFrame_.get());

	imguiManager_ = ImGuiManager::GetInstacne();
	imguiManager_->Init(winApp_->GetHwnd(), dxDevice_, dxCommon_->GetSwapChainBfCount(), dxHeap_->GetSRVHeap());
	EffectSystem::GetInstacne()->EditerInit(renderTarget_, dxHeap_, dxCmdList_, dxDevice_);
#endif

	render_->Init(dxCmdList_, dxDevice_, primitivePipeline_, graphicsCxt_->GetRenderTarget());
	audio_->Init();
	effectSystem_->Init();

	postProcess_->Init(dxDevice_, dxHeap_);

	Render::SetRenderTarget(RenderTargetType::Object3D_RenderTarget);

	GeometryFactory& geometryFactory = GeometryFactory::GetInstance();
	geometryFactory.Init();

	// -------------------------------------------------
	// ↓ その他初期化
	// -------------------------------------------------
	isFullScreen_ = false;
	runGame_ = true;
	
	Logger::Log("Engine Initialize compulete!\n");
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　終了処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::Finalize() {
	postProcess_->Finalize();
	audio_->Finalize();
	processedSceneFrame_->Finalize();
	computeShader_->Finalize();
	render_->Finalize();
	input_->Finalize();

#ifdef _DEBUG
	editorWindows_->Finalize();
	imguiManager_->Finalize();
#endif
	textureManager_->Finalize();
	graphicsCxt_->Finalize();
	winApp_->Finalize();
	CoUninitialize();
}

bool Engine::ProcessMessage() {
	return  winApp_->ProcessMessage();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::BeginFrame() {
	dxCommon_->Begin();
	input_->Update();

	if (Input::IsTriggerKey(DIK_F11)) {
		isFullScreen_ = !isFullScreen_;
		WinApp::GetInstance()->SetFullScreen(isFullScreen_);
	}
	Render::SetRenderTarget(RenderTargetType::Object3D_RenderTarget);

#ifdef _DEBUG
	imguiManager_->Begin();
	editorWindows_->Begin();
#endif // 

	//ax::NodeEditor::Begin("MyNodeEditor");
	//// ノードやリンクの定義をここに書く
	//// 例：
	//ax::NodeEditor::NodeId nodeId = 1;
	//ax::NodeEditor::BeginNode(nodeId);
	//ImGui::Text("Node 1");
	//ax::NodeEditor::EndNode();
	//ax::NodeEditor::End();

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::EndFrame() {
	Engine::RenderFrame();
	
#ifdef _DEBUG
	imguiManager_->End();
	imguiManager_->Draw(dxCmdList_);

	editorWindows_->End();
#endif

	dxCommon_->End();
	dxHeap_->FreeList();
	audio_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　1frameの結果を描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::RenderFrame() {
	// gameで使用したlineの描画を開始する
	if (editorWindows_->GetColliderDraw()) {
		ColliderCollector::GetInstance()->Draw();
	}

	if (editorWindows_->GetGridDraw()) {
		DrawGrid(render_->GetViewport3D(), render_->GetProjection3D());
	}
	
	primitivePipeline_->Draw(dxCmdList_);
	Render::PrimitiveDrawCall();

	// 最終Textureの作成
	BlendFinalTexture();

	postProcess_->Execute(dxCmdList_, processedSceneFrame_->GetResource());

	// guiの描画
#ifdef _DEBUG
	editorWindows_->Update();
#endif

	// swapChainの変更
	dxCommon_->SetSwapChain();
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Normal.json");
	processedSceneFrame_->Draw(dxCmdList_);

	renderTarget_->TransitionResource(dxCmdList_, Object3D_RenderTarget, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　最終的に描画するTextureを合成する
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::BlendFinalTexture() {
	// -------------------------------------------------
	// ↓ Resourceの状態を切り替える(obj3D, sprite2D, renderTexture)
	// -------------------------------------------------
	renderTarget_->TransitionResource(
		dxCmdList_,
		Object3D_RenderTarget,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);

	// 最終描画のTextureを書き込み可能状態にする
	processedSceneFrame_->TransitionResource(dxCmdList_,
									   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
									   D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// -------------------------------------------------
	// ↓ object3Dと最終描画のTextureを合成する
	// -------------------------------------------------
	computeShader_->BlendRenderTarget(dxCmdList_, renderTarget_->GetRenderTargetSRVHandle(Object3D_RenderTarget).handleGPU, processedSceneFrame_->GetUAV());

	// -------------------------------------------------
	// ↓ 映すTextureをpixeslShaderで使えるようにする
	// -------------------------------------------------
	processedSceneFrame_->TransitionResource(dxCmdList_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　生成する関数群
//////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<Sprite> Engine::CreateSprite(const std::string& fileName) {
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	sprite->Init(dxDevice_, fileName);
	return sprite;
}

std::unique_ptr<Model> Engine::CreateModel(const std::string& directoryPath, const std::string& filePath) {
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Init(dxDevice_, directoryPath, filePath);
	return model;
}

std::unique_ptr<WorldTransform> Engine::CreateWorldTransform() {
	std::unique_ptr<WorldTransform> result = std::make_unique<WorldTransform>();
	result->Init(dxDevice_);
	return result;
}

std::unique_ptr<Skinning> Engine::CreateSkinning(Skeleton* skeleton, Model* model, uint32_t index) {
	std::unique_ptr<Skinning> result = std::make_unique<Skinning>();
	result->CreateSkinCluster(dxDevice_, skeleton, model->GetMesh(index), dxHeap_, model->GetSkinClustersData(index));
	return result;
}

std::unique_ptr<Material> Engine::CreateMaterial(const Model::ModelMaterialData data) {
	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->Init(dxDevice_, data);
	return material;
}

std::unique_ptr<PBRMaterial> Engine::CreatePBRMaterial(const Model::ModelMaterialData data) {
	std::unique_ptr<PBRMaterial> material = std::make_unique<PBRMaterial>();
	material->Init(dxDevice_);
	material->SetMaterialData(data);
	return material;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　CSの設定
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::RunCS() {
	renderTarget_->TransitionResource(dxCmdList_, Object3D_RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	computeShader_->RunComputeShader(dxCmdList_);
}

void Engine::SetPSOPrimitive() {
	primitivePipeline_->Draw(dxCmdList_);
}

void Engine::SetPipeline(PSOType type, const std::string& typeName) {
	switch (type) {
	case PSOType::Object3d:
		graphicsPipeline_->SetPipeline(dxCmdList_, type, typeName);
		lastUsedPipeline_ = graphicsPipeline_->GetLastUsedPipeline();
		break;
	case PSOType::Sprite:
		graphicsPipeline_->SetPipeline(dxCmdList_, type, typeName);
		lastUsedPipeline_ = graphicsPipeline_->GetLastUsedPipeline();
		break;
	case PSOType::ProcessedScene:
		graphicsPipeline_->SetPipeline(dxCmdList_, type, typeName);
		lastUsedPipeline_ = graphicsPipeline_->GetLastUsedPipeline();
		break;
	case PSOType::Primitive:
		break;
	default:
		break;
	}
}

Pipeline* Engine::GetLastUsedPipeline() {
	return lastUsedPipeline_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　深度バッファをリセットする
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::ClearDepth() {
	renderTarget_->ClearDepth(dxCmdList_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　CSPipelineの設定
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::SetCsPipeline(const CsPipelineType& kind) {
	computeShader_->SetCsPipeline(kind, dxCmdList_);
}

void Engine::SetSkinning(Skinning* skinning, Mesh* mesh) {
	computeShader_->SetCsPipeline(CsPipelineType::Skinning_Pipeline, dxCmdList_);

	mesh->SetInitVertex();
	skinning->RunCs(dxCmdList_);
	skinning->EndCS(dxCmdList_, mesh);
}

void Engine::ResetComputeShader() {
	computeShader_->ResetComputeShader();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Sound系
//////////////////////////////////////////////////////////////////////////////////////////////////

SoundData Engine::LoadAudio(const std::string filePath) {
	return audio_->SoundLoad(filePath.c_str());
}

AudioData Engine::LoadAudio(const SoundData& loadAudioData) {
	return audio_->LoadAudio(loadAudioData);
}

void Engine::PlayAudio(const AudioData& soundData, bool isLoop, float volume, bool checkPlaying) {
	audio_->PlayAudio(soundData, isLoop, volume, checkPlaying);
}

void Engine::Pause(const AudioData& soundData) {
	audio_->PauseAudio(soundData.pSourceVoice);
}

void Engine::ReStart(const AudioData& soundData) {
	audio_->ReStartAudio(soundData.pSourceVoice);
}

void Engine::Stop(const AudioData& soundData) {
	audio_->StopAudio(soundData.pSourceVoice);
}

void Engine::SetVolume(const AudioData& soundData, float volume) {
	audio_->SetVolume(soundData.pSourceVoice, volume);
}

bool Engine::GetIsPlaying(const AudioData& soundData) {
	return audio_->IsPlaying(soundData.pSourceVoice);
}

void Engine::SingleShotPlay(const SoundData& loadAudioData, float volume) {
	audio_->SinglShotPlay(loadAudioData, volume);
}

bool Engine::GetRunGame() {
	return runGame_;
}

PostProcess* Engine::GetPostProcess() {
	return postProcess_.get();
}

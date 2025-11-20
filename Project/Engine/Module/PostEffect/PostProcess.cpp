#include "PostProcess.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Render.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/System/Manager/ImGuiManager.h"

PostProcess::~PostProcess() {
	Finalize();
}

void PostProcess::Finalize() {
	pingPongBuff_.reset();
	grayscale_.reset();
	radialBlur_.reset();
	glitchNoise_.reset();
	vignette_.reset();
	dissolve_.reset();
	toonMap_.reset();
	bloom_.reset();
	smoothing_.reset();
	gaussianFilter_.reset();
	luminanceOutline_.reset();
	depthOutline_.reset();
	motionBlur_.reset();
	effectList_.clear();
	depthStencilResource_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostProcess::Init(ID3D12Device* device, DescriptorHeap* descriptorHeap, RenderTarget* renderTarget, DxResourceManager* _resourceManager) {
	AttributeGui::SetName("Post Process");
	pingPongBuff_ = std::make_unique<PingPongBuffer>();
	pingPongBuff_->Init(device, descriptorHeap, _resourceManager);

	// -------------------------------------------------
	// ↓ 深度バッファの作成
	// -------------------------------------------------
	depthStencilResource_ = CreateDepthStencilTextureResource(device, WinApp::sWindowWidth, WinApp::sWindowHeight);
	// DSVの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	depthHandle_ = descriptorHeap->AllocateDSV();
	device->CreateDepthStencilView(depthStencilResource_.Get(), &desc, depthHandle_.handleCPU);

	// -------------------------------------------------
	// ↓ PostEffectの作成
	// -------------------------------------------------
	grayscale_ = std::make_shared<Grayscale>();
	grayscale_->Init();

	radialBlur_ = std::make_shared<RadialBlur>();
	radialBlur_->Init();
	radialBlur_->SetIsEnable(true);

	glitchNoise_ = std::make_shared<GlitchNoise>();
	glitchNoise_->Init();
	glitchNoise_->SetIsEnable(true);

	vignette_ = std::make_shared<Vignette>();
	vignette_->Init();

	dissolve_ = std::make_shared<Dissolve>();
	dissolve_->Init();

	toonMap_ = std::make_shared<ToonMap>();
	toonMap_->Init();
	toonMap_->SetIsEnable(true);

	bloom_ = std::make_shared<Bloom>();
	bloom_->Init();
	bloom_->SetPongResource(pingPongBuff_.get());
	bloom_->SetIsEnable(true);
	bloom_->SetDepthHandle(depthHandle_.handleCPU);

	smoothing_ = std::make_unique<Smoothing>();
	smoothing_->Init();
	
	gaussianFilter_ = std::make_shared<GaussianFilter>();
	gaussianFilter_->Init();
	
	luminanceOutline_ = std::make_shared<LuminanceBasedOutline>();
	luminanceOutline_->Init();
	
	depthOutline_ = std::make_shared<DepthBasedOutline>();
	depthOutline_->Init();
	
	motionBlur_ = std::make_shared<MotionBlur>();
	motionBlur_->Init();
	motionBlur_->SetMotionResource(renderTarget->GetRenderTargetResource(RenderTargetType::MotionVector_RenderTarget));
	motionBlur_->SetIsEnable(true);

	AddEffect(PostEffectType::RadialBlur);
	AddEffect(PostEffectType::GlitchNoise);
	AddEffect(PostEffectType::Vignette);
	AddEffect(PostEffectType::Dissolve);
	AddEffect(PostEffectType::LuminanceOutline);
	AddEffect(PostEffectType::DepthOutline);
	AddEffect(PostEffectType::Bloom);
	AddEffect(PostEffectType::MotionBlur);
	AddEffect(PostEffectType::Smoothing);
	AddEffect(PostEffectType::GaussianFilter);
	AddEffect(PostEffectType::Grayscale);
	AddEffect(PostEffectType::ToonMap);

	EditorWindows::AddObjectWindow(this, "Post Process");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行
///////////////////////////////////////////////////////////////////////////////////////////////

void PostProcess::Execute(ID3D12GraphicsCommandList* _commandList, DxResource* _dxResource) {
	std::vector<RenderTargetType> types(1, RenderTargetType::OffScreen_RenderTarget);
	Render::SetRenderTarget(types, depthHandle_);
	
	if (effectList_.empty()) {
		return;
	}
	
	// sceneのリソースをコピーする
	Copy(_commandList, _dxResource);
	// renderTargetをセットする
	pingPongBuff_->SetRenderTarget(_commandList, BufferType::Ping, depthHandle_.handleCPU);
	uint32_t cout = 0;
	// ポストエフェクトを実行する
	for (auto& effect : effectList_) {
		if (effect->GetIsEnable()) {
			effect->SetCommand(_commandList, pingPongBuff_->GetPingResource());

			pingPongBuff_->Swap(_commandList);
			pingPongBuff_->SetRenderTarget(_commandList, BufferType::Pong, depthHandle_.handleCPU);
			cout++;
		}
	}

	// resourceを入れ替える
	if (effectList_.size() % 2 == 0 && !effectList_.empty()) {
		pingPongBuff_->Swap(_commandList);
	}

	// 最終的な描画をシーンにコピーする
	PostCopy(_commandList, _dxResource);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コピーする
///////////////////////////////////////////////////////////////////////////////////////////////

void PostProcess::Copy(ID3D12GraphicsCommandList* _commandList, DxResource* _dxResource) {
	_dxResource->Transition(_commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	pingPongBuff_->Transition(_commandList, D3D12_RESOURCE_STATE_COPY_DEST, BufferType::Ping);
	_commandList->CopyResource(pingPongBuff_->GetPingResource()->GetResource(), _dxResource->GetResource());

	pingPongBuff_->Transition(_commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, BufferType::Pong);
}

void PostProcess::PostCopy(ID3D12GraphicsCommandList* _commandList, DxResource* _dxResource) {
	const bool isEven = (effectList_.size() % 2 == 0);
	auto* finalResource = isEven ? pingPongBuff_->GetPongResource() : pingPongBuff_->GetPingResource();

	// 遷移
	finalResource->Transition(_commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	_dxResource->Transition(_commandList, D3D12_RESOURCE_STATE_COPY_DEST);
	// コピー
	_commandList->CopyResource(_dxResource->GetResource(), finalResource->GetResource());
	// 元の状態に戻す
	_dxResource->Transition(_commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	pingPongBuff_->Transition(_commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, BufferType::Pong);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ effectの追加
///////////////////////////////////////////////////////////////////////////////////////////////

void PostProcess::AddEffect(PostEffectType type) {
	if (CheckAddEffect(type)) {
		addEffectList_.push_back(type);
		switch (type) {
		case PostEffectType::Grayscale:
			effectList_.push_back(grayscale_);
			break;
		case PostEffectType::RadialBlur:
			effectList_.push_back(radialBlur_);
			break;
		case PostEffectType::GlitchNoise:
			effectList_.push_back(glitchNoise_);
			break;
		case PostEffectType::Vignette:
			effectList_.push_back(vignette_);
			break;
		case PostEffectType::Dissolve:
			effectList_.push_back(dissolve_);
			break;
		case PostEffectType::ToonMap:
			effectList_.push_back(toonMap_);
			break;
		case PostEffectType::Bloom:
			effectList_.push_back(bloom_);
			break;
		case PostEffectType::Smoothing:
			effectList_.push_back(smoothing_);
			break;
		case PostEffectType::GaussianFilter:
			effectList_.push_back(gaussianFilter_);
			break;
		case PostEffectType::LuminanceOutline:
			effectList_.push_back(luminanceOutline_);
			break;
		case PostEffectType::DepthOutline:
			effectList_.push_back(depthOutline_);
			break;
		case PostEffectType::MotionBlur:
			effectList_.push_back(motionBlur_);
			break;
		default:
			break;
		}
	}
}

bool PostProcess::CheckAddEffect(PostEffectType type) {
	for (const auto& effect : addEffectList_) {
		if (effect == type) {
			return false;
		}
	}
	return true;
}

std::shared_ptr<IPostEffect> PostProcess::GetEffect(PostEffectType type) {
	switch (type) {
	case PostEffectType::Grayscale:
		return grayscale_;
	case PostEffectType::RadialBlur:
		return radialBlur_;
	case PostEffectType::GlitchNoise:
		return glitchNoise_;
	case PostEffectType::Vignette:
		return vignette_;
	case PostEffectType::Dissolve:
		return dissolve_;
	case PostEffectType::ToonMap:
		return toonMap_;
	case PostEffectType::Bloom:
		return bloom_;
	case PostEffectType::Smoothing:
		return smoothing_;
	case PostEffectType::GaussianFilter:
		return gaussianFilter_;
	case PostEffectType::LuminanceOutline:
		return luminanceOutline_;
	case PostEffectType::DepthOutline:
		return depthOutline_;
	case PostEffectType::MotionBlur:
		return motionBlur_;
	default:
		return nullptr;
	}
}

void PostProcess::Debug_Gui() {
	if (ImGui::CollapsingHeader("CheckList")) {
		for (auto& effect : effectList_) {
			effect->CheckBox();
		}
	}
	ImGui::Separator();
	for (auto& effect : effectList_) {
		effect->Debug_Gui();
	}
}

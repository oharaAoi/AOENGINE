#include "PostProcess.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Render.h"
#include "imgui.h"

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

void PostProcess::Init(ID3D12Device* device, DescriptorHeap* descriptorHeap, RenderTarget* renderTarget) {
	AttributeGui::SetName("Post Process");
	pingPongBuff_ = std::make_unique<PingPongBuffer>();
	pingPongBuff_->Init(device, descriptorHeap);

	// -------------------------------------------------
	// ↓ 深度バッファの作成
	// -------------------------------------------------
	depthStencilResource_ = CreateDepthStencilTextureResource(device, kWindowWidth_, kWindowHeight_);
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

	AddEffect(PostEffectType::RADIALBLUR);
	AddEffect(PostEffectType::GLITCHNOISE);
	AddEffect(PostEffectType::VIGNETTE);
	AddEffect(PostEffectType::DISSOLVE);
	AddEffect(PostEffectType::LUMINANCE_OUTLINE);
	AddEffect(PostEffectType::DEPTH_OUTLINE);
	AddEffect(PostEffectType::BLOOM);
	AddEffect(PostEffectType::MOTIONBLUR);
	AddEffect(PostEffectType::SMOOTHING);
	AddEffect(PostEffectType::GAUSSIANFILTER);
	AddEffect(PostEffectType::GRAYSCALE);
	AddEffect(PostEffectType::TOONMAP);

	EditorWindows::AddObjectWindow(this, "Post Process");
}

void PostProcess::Execute(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource) {
	std::vector<RenderTargetType> types(1, RenderTargetType::OffScreen_RenderTarget);
	Render::SetRenderTarget(types, depthHandle_);
	
	if (effectList_.empty()) {
		return;
	}
	
	Copy(commandList, shaderResource);

	pingPongBuff_->SetRenderTarget(commandList, BufferType::PONG, depthHandle_.handleCPU);
	uint32_t cout = 0;
	for (auto& effect : effectList_) {
		if (effect->GetIsEnable()) {
			effect->SetCommand(commandList, pingPongBuff_->GetPingResource());

			pingPongBuff_->Swap(commandList);
			pingPongBuff_->SetRenderTarget(commandList, BufferType::PONG, depthHandle_.handleCPU);
			cout++;
		}
	}

	if (effectList_.size() % 2 == 0 && !effectList_.empty()) {
		pingPongBuff_->Swap(commandList);
	}

	
	PostCopy(commandList, shaderResource);
}

void PostProcess::Copy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource) {
	shaderResource->Transition(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	pingPongBuff_->Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST, BufferType::PING);
	commandList->CopyResource(pingPongBuff_->GetPingResource()->GetResource(), shaderResource->GetResource());

	pingPongBuff_->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, BufferType::PING);
}

void PostProcess::PostCopy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource) {
	const bool isEven = (effectList_.size() % 2 == 0);
	auto* finalResource = isEven ? pingPongBuff_->GetPongResource() : pingPongBuff_->GetPingResource();

	// 遷移
	finalResource->Transition(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	shaderResource->Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
	// コピー
	commandList->CopyResource(shaderResource->GetResource(), finalResource->GetResource());
	// 元の状態に戻す
	shaderResource->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	pingPongBuff_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, BufferType::PONG);
}

void PostProcess::AddEffect(PostEffectType type) {
	if (CheckAddEffect(type)) {
		addEffectList_.push_back(type);
		switch (type) {
		case PostEffectType::GRAYSCALE:
			effectList_.push_back(grayscale_);
			break;
		case PostEffectType::RADIALBLUR:
			effectList_.push_back(radialBlur_);
			break;
		case PostEffectType::GLITCHNOISE:
			effectList_.push_back(glitchNoise_);
			break;
		case PostEffectType::VIGNETTE:
			effectList_.push_back(vignette_);
			break;
		case PostEffectType::DISSOLVE:
			effectList_.push_back(dissolve_);
			break;
		case PostEffectType::TOONMAP:
			effectList_.push_back(toonMap_);
			break;
		case PostEffectType::BLOOM:
			effectList_.push_back(bloom_);
			break;
		case PostEffectType::SMOOTHING:
			effectList_.push_back(smoothing_);
			break;
		case PostEffectType::GAUSSIANFILTER:
			effectList_.push_back(gaussianFilter_);
			break;
		case PostEffectType::LUMINANCE_OUTLINE:
			effectList_.push_back(luminanceOutline_);
			break;
		case PostEffectType::DEPTH_OUTLINE:
			effectList_.push_back(depthOutline_);
			break;
		case PostEffectType::MOTIONBLUR:
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
	case PostEffectType::GRAYSCALE:
		return grayscale_;
		break;
	case PostEffectType::RADIALBLUR:
		return radialBlur_;
		break;
	case PostEffectType::GLITCHNOISE:
		return glitchNoise_;
		break;
	case PostEffectType::VIGNETTE:
		return vignette_;
		break;
	case PostEffectType::DISSOLVE:
		return dissolve_;
		break;
	case PostEffectType::TOONMAP:
		return toonMap_;
		break;
	case PostEffectType::BLOOM:
		return bloom_;
		break;
	case PostEffectType::SMOOTHING:
		return smoothing_;
		break;
	case PostEffectType::GAUSSIANFILTER:
		return gaussianFilter_;
		break;
	case PostEffectType::LUMINANCE_OUTLINE:
		return luminanceOutline_;
		break;
	case PostEffectType::DEPTH_OUTLINE:
		return depthOutline_;
		break;
	case PostEffectType::MOTIONBLUR:
		return motionBlur_;
		break;
	default:
		return nullptr;
		break;
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

#include "PostProcess.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

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
	effectList_.clear();
}

void PostProcess::Init(ID3D12Device* device, DescriptorHeap* descriptorHeap) {
	AttributeGui::SetName("Post Process");
	pingPongBuff_ = std::make_unique<PingPongBuffer>();

	pingPongBuff_->Init(device, descriptorHeap);

	grayscale_ = std::make_shared<Grayscale>();
	grayscale_->Init();

	radialBlur_ = std::make_shared<RadialBlur>();
	radialBlur_->Init();

	glitchNoise_ = std::make_shared<GlitchNoise>();
	glitchNoise_->Init();

	vignette_ = std::make_shared<Vignette>();
	vignette_->Init();

	dissolve_ = std::make_shared<Dissolve>();
	dissolve_->Init();

	AddEffect(PostEffectType::RADIALBLUR);
	AddEffect(PostEffectType::GLITCHNOISE);
	//AddEffect(PostEffectType::DISSOLVE);

#ifdef _DEBUG
	EditorWindows::AddObjectWindow(this, "Post Process");
#endif
}

void PostProcess::Execute(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource) {
	if (effectList_.empty()) {
		return;
	}
	
	Copy(commandList, shaderResource);

	pingPongBuff_->SetRenderTarget(commandList, BufferType::PONG);
	uint32_t cout = 0;
	for (auto& effect : effectList_) {
		effect->SetCommand(commandList, pingPongBuff_->GetPingResource());


		pingPongBuff_->Swap(commandList);
		pingPongBuff_->SetRenderTarget(commandList, BufferType::PONG);
		cout++;
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
	default:
		return nullptr;
		break;
	}
}

#ifdef _DEBUG
void PostProcess::Debug_Gui() {
	radialBlur_->Debug_Gui();
	glitchNoise_->Debug_Gui();
	vignette_->Debug_Gui();
	dissolve_->Debug_Gui();
}
#endif
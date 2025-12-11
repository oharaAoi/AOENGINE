#include "PostProcess.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Render.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Module/PostEffect/Grayscale.h"
#include "Engine/Module/PostEffect/RadialBlur.h"
#include "Engine/Module/PostEffect/GlitchNoise.h"
#include "Engine/Module/PostEffect/Vignette.h"
#include "Engine/Module/PostEffect/Dissolve.h"
#include "Engine/Module/PostEffect/ToonMap.h"
#include "Engine/Module/PostEffect/Bloom.h"
#include "Engine/Module/PostEffect/Smoothing.h"
#include "Engine/Module/PostEffect/GaussianFilter.h"
#include "Engine/Module/PostEffect/LuminanceBasedOutline.h"
#include "Engine/Module/PostEffect/DepthBasedOutline.h"
#include "Engine/Module/PostEffect/MotionBlur.h"
#include <utility>

using namespace AOENGINE;
using namespace PostEffect;

AOENGINE::PostProcess::~PostProcess() {
	Finalize();
}

void AOENGINE::PostProcess::Finalize() {
	pingPongBuff_.reset();
	effectMap_.clear();
	effectList_.clear();
	depthStencilResource_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PostProcess::Init(ID3D12Device* device, AOENGINE::DescriptorHeap* descriptorHeap, AOENGINE::RenderTarget* renderTarget, AOENGINE::DxResourceManager* _resourceManager) {
	AOENGINE::AttributeGui::SetName("Post Process");
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

	pMotionBluerRenderTarget_ = renderTarget->GetRenderTargetResource(RenderTargetType::MotionVector_RenderTarget);

	RegisterEffect<Grayscale>(PostEffectType::Grayscale);
	RegisterEffect<RadialBlur>(PostEffectType::RadialBlur);
	RegisterEffect<GlitchNoise>(PostEffectType::GlitchNoise);
	RegisterEffect<Vignette>(PostEffectType::Vignette);
	RegisterEffect<Dissolve>(PostEffectType::Dissolve);
	RegisterEffect<ToonMap>(PostEffectType::ToonMap);
	RegisterEffect<Bloom>(PostEffectType::Bloom);
	RegisterEffect<Smoothing>(PostEffectType::Smoothing);
	RegisterEffect<GaussianFilter>(PostEffectType::GaussianFilter);
	RegisterEffect<LuminanceBasedOutline>(PostEffectType::LuminanceOutline);
	RegisterEffect<DepthBasedOutline>(PostEffectType::DepthOutline);
	RegisterEffect<MotionBlur>(PostEffectType::MotionBlur);

	effectMap_[PostEffectType::Bloom]->PostInit(this);
	effectMap_[PostEffectType::MotionBlur]->PostInit(this);

	effectMap_[PostEffectType::RadialBlur]->SetIsEnable(true);
	effectMap_[PostEffectType::GlitchNoise]->SetIsEnable(true);
	effectMap_[PostEffectType::ToonMap]->SetIsEnable(true);
	effectMap_[PostEffectType::MotionBlur]->SetIsEnable(true);

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

	AOENGINE::EditorWindows::AddObjectWindow(this, "Post Process");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PostProcess::Execute(ID3D12GraphicsCommandList* _commandList, AOENGINE::DxResource* _dxResource) {
	std::vector<RenderTargetType> types(1, RenderTargetType::OffScreen_RenderTarget);
	AOENGINE::Render::SetRenderTarget(types, depthHandle_);
	
	if (effectList_.empty()) {
		return;
	}
	
	// sceneのリソースをコピーする
	Copy(_commandList, _dxResource);
	// renderTargetをセットする
	pingPongBuff_->SetRenderTarget(_commandList, BufferType::Pong, depthHandle_.handleCPU);
	uint32_t cout = 0;
	// ポストエフェクトを実行する
	for (auto& effect : effectList_) {
		if (effectMap_[effect]->GetIsEnable()) {
			effectMap_[effect]->SetCommand(_commandList, pingPongBuff_->GetPingResource());

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

void AOENGINE::PostProcess::Copy(ID3D12GraphicsCommandList* _commandList, AOENGINE::DxResource* _dxResource) {
	_dxResource->Transition(_commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	pingPongBuff_->Transition(_commandList, D3D12_RESOURCE_STATE_COPY_DEST, BufferType::Ping);
	_commandList->CopyResource(pingPongBuff_->GetPingResource()->GetResource(), _dxResource->GetResource());

	pingPongBuff_->Transition(_commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, BufferType::Ping);
}

void AOENGINE::PostProcess::PostCopy(ID3D12GraphicsCommandList* _commandList, AOENGINE::DxResource* _dxResource) {
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

void AOENGINE::PostProcess::AddEffect(PostEffectType type) {
	if (!CheckAddEffect(type)) return;

	addEffectList_.push_back(type);

	if (effectMap_.count(type)) {
		effectList_.push_back(type); // type だけを追加
	}
}

bool AOENGINE::PostProcess::CheckAddEffect(PostEffectType type) {
	for (const auto& effect : addEffectList_) {
		if (effect == type) {
			return false;
		}
	}
	return true;
}

std::shared_ptr<IPostEffect> AOENGINE::PostProcess::GetEffect(PostEffectType type) {
	auto it = effectMap_.find(type);
	return (it != effectMap_.end()) ? it->second : nullptr;
}

void AOENGINE::PostProcess::Debug_Gui() {
	if (ImGui::CollapsingHeader("CheckList")) {
		for (const auto& [type, effect] : effectMap_) {
			effect->CheckBox();
		}
	}

	ImGui::Separator();

	for (auto t : effectList_) {
		effectMap_[t]->Debug_Gui();
	}
}

#include "PostProcess.h"
#include "Engine/Render/Render.h"
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
#include "Engine/Lib/Json/IJsonConverter.h"
#include <algorithm>
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
	descriptorHeap_ = descriptorHeap;
	
	pingPongBuff_ = std::make_unique<PingPongBuffer>();
	pingPongBuff_->Init(device, descriptorHeap, _resourceManager);

	// -------------------------------------------------
	// ↓ 深度バッファの作成
	// -------------------------------------------------
	depthStencilResource_ = CreateDepthStencilTextureResource(device, WinApp::sClientWidth, WinApp::sClientHeight);
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

	// マップにポインタを追加
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

	// 行なう順番に追加
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

	defaultSettings_ = Serialize();
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
	// ポストエフェクトを実行する
	for (auto& effect : effectList_) {
		if (effectMap_[effect]->GetIsEnable()) {
			effectMap_[effect]->SetCommand(_commandList, pingPongBuff_->GetPingResource());

			pingPongBuff_->Swap(_commandList);
			pingPongBuff_->SetRenderTarget(_commandList, BufferType::Pong, depthHandle_.handleCPU);
		}
	}

	// resourceを入れ替える
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ frame終了時のコピー
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PostProcess::PostCopy(ID3D12GraphicsCommandList* _commandList, AOENGINE::DxResource* _dxResource) {
	// Swap後は常にPingが直前のエフェクト出力を指す。
	auto* finalResource = pingPongBuff_->GetPingResource();

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
// ↓ バッファをクリアする
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PostProcess::ClearBuffer() {
	// Bloom/MotionBlurはResizeBufferで再生成されるため、破棄前の設定を退避する。
	resizeSettings_ = Serialize();
	pingPongBuff_.reset();

	depthStencilResource_.Reset();
	descriptorHeap_->FreeDSV(depthHandle_.assignIndex_);

	effectMap_[PostEffectType::Bloom].reset();
	effectMap_[PostEffectType::MotionBlur].reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ effectの追加
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PostProcess::ResizeBuffer(ID3D12Device* device, AOENGINE::RenderTarget* renderTarget, AOENGINE::DxResourceManager* _resourceManager) {
	pingPongBuff_ = std::make_unique<PingPongBuffer>();
	pingPongBuff_->Init(device, descriptorHeap_, _resourceManager);

	depthStencilResource_ = CreateDepthStencilTextureResource(device, WinApp::sClientWidth, WinApp::sClientHeight);
	// DSVの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	depthHandle_ = descriptorHeap_->AllocateDSV();
	device->CreateDepthStencilView(depthStencilResource_.Get(), &desc, depthHandle_.handleCPU);

	pMotionBluerRenderTarget_ = renderTarget->GetRenderTargetResource(RenderTargetType::MotionVector_RenderTarget);

	effectMap_[PostEffectType::Bloom] = std::make_shared<Bloom>();
	effectMap_[PostEffectType::Bloom]->Init();

	effectMap_[PostEffectType::MotionBlur] = std::make_shared<MotionBlur>();
	effectMap_[PostEffectType::MotionBlur]->Init();

	effectMap_[PostEffectType::Bloom]->PostInit(this);
	effectMap_[PostEffectType::MotionBlur]->PostInit(this);

	if (!resizeSettings_.is_null()) {
		Deserialize(resizeSettings_);
		resizeSettings_ = json();
	}
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ effect追加のチェックリスト
///////////////////////////////////////////////////////////////////////////////////////////////

bool AOENGINE::PostProcess::CheckAddEffect(PostEffectType type) {
	for (const auto& effect : addEffectList_) {
		if (effect == type) {
			return false;
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ effectの取得
///////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<IPostEffect> AOENGINE::PostProcess::GetEffect(PostEffectType type) const {
	auto it = effectMap_.find(type);
	return (it != effectMap_.end()) ? it->second : nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Effect名
///////////////////////////////////////////////////////////////////////////////////////////////

const char* AOENGINE::PostProcess::GetEffectName(PostEffectType type) {
	switch (type) {
	case PostEffectType::Grayscale: return "Grayscale";
	case PostEffectType::RadialBlur: return "RadialBlur";
	case PostEffectType::GlitchNoise: return "GlitchNoise";
	case PostEffectType::Vignette: return "Vignette";
	case PostEffectType::Dissolve: return "Dissolve";
	case PostEffectType::ToonMap: return "ToonMap";
	case PostEffectType::Bloom: return "Bloom";
	case PostEffectType::Smoothing: return "Smoothing";
	case PostEffectType::GaussianFilter: return "GaussianFilter";
	case PostEffectType::LuminanceOutline: return "LuminanceOutline";
	case PostEffectType::DepthOutline: return "DepthOutline";
	case PostEffectType::MotionBlur: return "MotionBlur";
	default: return "Unknown";
	}
}

std::optional<PostEffectType> AOENGINE::PostProcess::FindEffectType(std::string_view name) {
	for (PostEffectType type : {
		PostEffectType::Grayscale, PostEffectType::RadialBlur, PostEffectType::GlitchNoise,
		PostEffectType::Vignette, PostEffectType::Dissolve, PostEffectType::ToonMap,
		PostEffectType::Bloom, PostEffectType::Smoothing, PostEffectType::GaussianFilter,
		PostEffectType::LuminanceOutline, PostEffectType::DepthOutline, PostEffectType::MotionBlur }) {
		if (name == GetEffectName(type)) { return type; }
	}
	return std::nullopt;
}

namespace {

json SerializeConverter(const AOENGINE::IJsonConverter& converter) {
	json wrapped = converter.ToJson("parameters");
	json parameters = wrapped.value("parameters", json::object());
	parameters.erase("isEnable");
	return parameters;
}

void DeserializeConverter(AOENGINE::IJsonConverter& converter, const json& parameters) {
	converter.FromJson(json{ { "parameters", parameters } });
}

}

json AOENGINE::PostProcess::Serialize() const {
	json effects = json::array();
	for (PostEffectType type : effectList_) {
		const auto effect = GetEffect(type);
		if (!effect || !effect->GetSettingsConverter()) { continue; }

		json parameters = SerializeConverter(*effect->GetSettingsConverter());
		if (type == PostEffectType::Bloom) {
			if (const auto bloom = std::dynamic_pointer_cast<Bloom>(effect)) {
				parameters["brightnessThreshold"] = SerializeConverter(*bloom->GetBrightnessThreshold().GetSettingsConverter());
				parameters["blurWidth"] = SerializeConverter(*bloom->GetBlurWidth().GetSettingsConverter());
				parameters["blurHeight"] = SerializeConverter(*bloom->GetBlurHeight().GetSettingsConverter());
			}
		}
		effects.push_back({
			{ "type", GetEffectName(type) },
			{ "enabled", effect->GetIsEnable() },
			{ "parameters", std::move(parameters) }
		});
	}
	return { { "version", 1 }, { "effects", std::move(effects) } };
}

bool AOENGINE::PostProcess::Deserialize(const json& data) {
	if (!data.is_object() || data.value("version", 0) != 1 ||
		!data.contains("effects") || !data.at("effects").is_array()) {
		return false;
	}

	const std::vector<PostEffectType> registeredOrder = effectList_;
	for (PostEffectType type : registeredOrder) {
		if (const auto effect = GetEffect(type)) { effect->SetIsEnable(false); }
	}

	std::vector<PostEffectType> loadedOrder;
	for (const json& entry : data.at("effects")) {
		if (!entry.is_object()) { continue; }
		const auto type = FindEffectType(entry.value("type", ""));
		if (!type) { continue; }
		const auto effect = GetEffect(*type);
		if (!effect || !effect->GetSettingsConverter()) { continue; }
		if (std::find(loadedOrder.begin(), loadedOrder.end(), *type) != loadedOrder.end()) { continue; }
		loadedOrder.push_back(*type);

		effect->SetIsEnable(entry.value("enabled", false));
		const json parameters = entry.value("parameters", json::object());
		DeserializeConverter(*effect->GetSettingsConverter(), parameters);
		effect->ApplySaveSettings();

		if (*type == PostEffectType::Bloom) {
			if (const auto bloom = std::dynamic_pointer_cast<Bloom>(effect)) {
				if (parameters.contains("brightnessThreshold")) {
					DeserializeConverter(*bloom->GetBrightnessThreshold().GetSettingsConverter(), parameters.at("brightnessThreshold"));
					bloom->GetBrightnessThreshold().ApplySaveSettings();
				}
				if (parameters.contains("blurWidth")) {
					DeserializeConverter(*bloom->GetBlurWidth().GetSettingsConverter(), parameters.at("blurWidth"));
					bloom->GetBlurWidth().ApplySaveSettings();
				}
				if (parameters.contains("blurHeight")) {
					DeserializeConverter(*bloom->GetBlurHeight().GetSettingsConverter(), parameters.at("blurHeight"));
					bloom->GetBlurHeight().ApplySaveSettings();
				}
			}
		}
	}
	for (PostEffectType type : registeredOrder) {
		if (std::find(loadedOrder.begin(), loadedOrder.end(), type) == loadedOrder.end()) {
			loadedOrder.push_back(type);
		}
	}
	if (!loadedOrder.empty()) { effectList_ = std::move(loadedOrder); }
	return true;
}

void AOENGINE::PostProcess::ResetToDefaults() {
	if (!defaultSettings_.is_null()) { Deserialize(defaultSettings_); }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PostProcess::Save(const std::string& rootField) {
	for (auto t : effectList_) {
		effectMap_[t]->Save(rootField);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PostProcess::Load(const std::string& rootField) {
	for (auto t : effectList_) {
		effectMap_[t]->Load(rootField);
	}
}

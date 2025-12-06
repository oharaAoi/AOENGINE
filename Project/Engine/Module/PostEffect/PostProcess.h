#pragma once
#include <memory>
#include <list>
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

#include "Engine/Module/PostEffect/PingPongBuffer.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Resource/DxResourceManager.h"
#include "Engine/DirectX/RTV/RenderTarget.h"

enum class PostEffectType {
	Grayscale,
	RadialBlur,
	GlitchNoise,
	Vignette,
	Dissolve,
	ToonMap,
	Bloom,
	Smoothing,
	GaussianFilter,
	LuminanceOutline,
	DepthOutline,
	MotionBlur,
};

namespace AOENGINE {

/// <summary>
/// postEffectなどを行うクラス
/// </summary>
class PostProcess :
	public AOENGINE::AttributeGui {
public:

	PostProcess() = default;
	~PostProcess();

public:

	// 終了
	void Finalize();
	// 初期化
	void Init(ID3D12Device* device, AOENGINE::DescriptorHeap* descriptorHeap, AOENGINE::RenderTarget* renderTarget, AOENGINE::DxResourceManager* _resourceManager);
	// 実行
	void Execute(ID3D12GraphicsCommandList* _commandList, AOENGINE::DxResource* _dxResource);
	// コピー
	void Copy(ID3D12GraphicsCommandList* _commandList, AOENGINE::DxResource* _dxResource);
	// 後で行うコピー
	void PostCopy(ID3D12GraphicsCommandList* _commandList, AOENGINE::DxResource* _dxResource);

	/// <summary>
	/// effectの追加
	/// </summary>
	/// <param name="type"></param>
	void AddEffect(PostEffectType type);

	/// <summary>
	/// effetの有無
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	bool CheckAddEffect(PostEffectType type);

	/// <summary>
	/// effectのポインタを返す
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	std::shared_ptr<PostEffect::IPostEffect> GetEffect(PostEffectType type);

	void Debug_Gui() override;

	std::shared_ptr<PostEffect::Grayscale> GetGrayscale() { return grayscale_; }
	std::shared_ptr<PostEffect::RadialBlur> GetRadialBlur() { return radialBlur_; }
	std::shared_ptr<PostEffect::GlitchNoise> GetGlitchNoise() { return glitchNoise_; }
	std::shared_ptr<PostEffect::Vignette> GetVignette() { return vignette_; }
	std::shared_ptr<PostEffect::Dissolve> GetDissolve() { return dissolve_; }

private:

	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	DescriptorHandles depthHandle_;

	std::unique_ptr<AOENGINE::PingPongBuffer> pingPongBuff_;

	std::shared_ptr<PostEffect::Grayscale> grayscale_;
	std::shared_ptr<PostEffect::RadialBlur> radialBlur_;
	std::shared_ptr<PostEffect::GlitchNoise> glitchNoise_;
	std::shared_ptr<PostEffect::Vignette> vignette_;
	std::shared_ptr<PostEffect::Dissolve> dissolve_;
	std::shared_ptr<PostEffect::ToonMap> toonMap_;
	std::shared_ptr<PostEffect::Bloom> bloom_;
	std::shared_ptr<PostEffect::Smoothing> smoothing_;
	std::shared_ptr<PostEffect::GaussianFilter> gaussianFilter_;
	std::shared_ptr<PostEffect::LuminanceBasedOutline> luminanceOutline_;
	std::shared_ptr<PostEffect::DepthBasedOutline> depthOutline_;
	std::shared_ptr<PostEffect::MotionBlur> motionBlur_;

	std::list<std::shared_ptr<PostEffect::IPostEffect>> effectList_;
	std::list<PostEffectType> addEffectList_;

};

}
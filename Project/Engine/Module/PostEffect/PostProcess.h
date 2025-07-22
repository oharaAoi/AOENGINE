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
#include "Engine/DirectX/Resource/ShaderResource.h"
#include <DirectX/RTV/RenderTarget.h>

enum class PostEffectType {
	GRAYSCALE,
	RADIALBLUR,
	GLITCHNOISE,
	VIGNETTE,
	DISSOLVE,
	TOONMAP,
	BLOOM,
	SMOOTHING,
	GAUSSIANFILTER,
	LUMINANCE_OUTLINE,
	DEPTH_OUTLINE,
	MOTIONBLUR,
};

/// <summary>
/// postEffectなどを行うクラス
/// </summary>
class PostProcess :
	public AttributeGui {
public:

	PostProcess() = default;
	~PostProcess();

	void Finalize();

	void Init(ID3D12Device* device, DescriptorHeap* descriptorHeap, RenderTarget* renderTarget);

	void Execute(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

	void Copy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

	void PostCopy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

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
	std::shared_ptr<IPostEffect> GetEffect(PostEffectType type);

	void Debug_Gui() override;

	std::shared_ptr<Grayscale> GetGrayscale() { return grayscale_; }
	std::shared_ptr<RadialBlur> GetRadialBlur() { return radialBlur_; }
	std::shared_ptr<GlitchNoise> GetGlitchNoise() { return glitchNoise_; }
	std::shared_ptr<Vignette> GetVignette() { return vignette_; }
	std::shared_ptr<Dissolve> GetDissolve() { return dissolve_; }

private:

	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	DescriptorHandles depthHandle_;

	std::unique_ptr<PingPongBuffer> pingPongBuff_;

	std::shared_ptr<Grayscale> grayscale_;
	std::shared_ptr<RadialBlur> radialBlur_;
	std::shared_ptr<GlitchNoise> glitchNoise_;
	std::shared_ptr<Vignette> vignette_;
	std::shared_ptr<Dissolve> dissolve_;
	std::shared_ptr<ToonMap> toonMap_;
	std::shared_ptr<Bloom> bloom_;
	std::shared_ptr<Smoothing> smoothing_;
	std::shared_ptr<GaussianFilter> gaussianFilter_;
	std::shared_ptr<LuminanceBasedOutline> luminanceOutline_;
	std::shared_ptr<DepthBasedOutline> depthOutline_;
	std::shared_ptr<MotionBlur> motionBlur_;

	std::list<std::shared_ptr<IPostEffect>> effectList_;
	std::list<PostEffectType> addEffectList_;

};
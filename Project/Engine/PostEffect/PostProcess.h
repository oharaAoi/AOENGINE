#pragma once
#include <memory>
#include <list>
#include "Engine/PostEffect/Grayscale.h"
#include "Engine/PostEffect/RadialBlur.h"
#include "Engine/PostEffect/GlitchNoise.h"
#include "Engine/PostEffect/PingPongBuffer.h"
#include "Engine/DirectX/Resource/ShaderResource.h"
#include "Engine/Components/Attribute/AttributeGui.h"

enum class PostEffectType {
	GRAYSCALE,
	RADIALBLUR,
	GLITCHNOISE,
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

	void Init(ID3D12Device* device, DescriptorHeap* descriptorHeap);

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

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

private:

	std::unique_ptr<PingPongBuffer> pingPongBuff_;

	std::shared_ptr<Grayscale> grayscale_;
	std::shared_ptr<RadialBlur> radialBlur_;
	std::shared_ptr<GlitchNoise> glitchNoise_;

	std::list<std::shared_ptr<IPostEffect>> effectList_;
	std::list<PostEffectType> addEffectList_;

};
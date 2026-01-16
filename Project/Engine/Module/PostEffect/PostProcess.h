#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include "Engine/Module/PostEffect/IPostEffect.h"

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
	/// effetの有無
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	bool CheckAddEffect(PostEffectType type);

	/// <summary>
	/// 編集項目
	/// </summary>
	void Debug_Gui() override;

	/// <summary>
	/// バッファをクリアする
	/// </summary>
	void ClearBuffer();

	/// <summary>
	/// バッファをリサイズする
	/// </summary>
	void ResizeBuffer(ID3D12Device* device, AOENGINE::DxResourceManager* _resourceManager);

public:


	/// <summary>
	/// effectのポインタを返す
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	std::shared_ptr<PostEffect::IPostEffect> GetEffect(PostEffectType type);

	template<class T>
	std::shared_ptr<T> GetEffectAs(PostEffectType type) {
		return std::dynamic_pointer_cast<T>(GetEffect(type));
	}

	AOENGINE::DxResource* GetMotionBluerRnderTarget() const { return pMotionBluerRenderTarget_; }
	AOENGINE::PingPongBuffer* GetPingPongBuffer() const { return pingPongBuff_.get(); }
	const DescriptorHandles& GetDepthHandle() const { return depthHandle_; }

private:

	/// <summary>
	/// effectの登録
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="type"></param>
	template<class T>
	void RegisterEffect(PostEffectType type) {
		auto ptr = std::make_shared<T>();
		ptr->Init();
		effectMap_[type] = ptr;
	}

	/// <summary>
	/// effectの追加
	/// </summary>
	/// <param name="type"></param>
	void AddEffect(PostEffectType type);


private:
	AOENGINE::DescriptorHeap* descriptorHeap_;
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	DescriptorHandles depthHandle_;

	std::unique_ptr<AOENGINE::PingPongBuffer> pingPongBuff_;

	AOENGINE::DxResource* pMotionBluerRenderTarget_ = nullptr;

	std::unordered_map<PostEffectType, std::shared_ptr<PostEffect::IPostEffect>> effectMap_;

	std::vector<PostEffectType> effectList_;     // 実行順
	std::vector<PostEffectType> addEffectList_;  // 重複登録防止

};

}
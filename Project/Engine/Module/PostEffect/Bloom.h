#pragma once
#include "Engine/Module/PostEffect/BrightnessThreshold.h"
#include "Engine/Module/PostEffect/GaussianBlurWidth.h"
#include "Engine/Module/PostEffect/GaussianBlurHeight.h"
#include "Engine/Module/PostEffect/PingPongBuffer.h"
#include "Engine/Lib/Math/Vector2.h"

namespace PostEffect {

/// <summary>
/// Bloomクラス
/// </summary>
class Bloom :
	public IPostEffect {
public:	// 構造体

	struct BloomSettings {
		float bloomIntensity;
	};
public:

	Bloom() = default;
	~Bloom() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスを表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

public:

	void SetPongResource(AOENGINE::PingPongBuffer* _resource) { postProcessResource_ = _resource; }

	void SetDepthHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { depthHandle_ = handle; }

private:

	std::unique_ptr<AOENGINE::PingPongBuffer> pingPongBuff_;

	std::unique_ptr<PostEffect::BrightnessThreshold> brightnessBuffer_;
	std::unique_ptr<PostEffect::GaussianBlurWidth> blurWidthBuffer_;
	std::unique_ptr<PostEffect::GaussianBlurHeight> blurHeightBuffer_;

	AOENGINE::DxResource* settingBuffer_;
	BloomSettings* setting_;

	AOENGINE::PingPongBuffer* postProcessResource_;
	AOENGINE::DxResource* sceneBuffer_;


	ID3D12Device* device_ = nullptr;
	AOENGINE::DescriptorHeap* dxHeap_ = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE depthHandle_;
};

}
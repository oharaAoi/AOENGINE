#pragma once
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Resource/DxResourceManager.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

/// <summary>
/// Sceneを描画した後のResourceを所有
/// </summary>
class ProcessedSceneFrame {
public:

	ProcessedSceneFrame() = default;
	~ProcessedSceneFrame() = default;

	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dxHeap"></param>
	void Init(AOENGINE::DxResourceManager* _dxResourceManager);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// Sceneの描画
	/// </summary>
	void DrawScene();

	/// <summary>
	/// Resourceの状態を遷移させる
	/// </summary>
	/// <param name="commandList">: commandList</param>
	/// <param name="beforState">: 今の状態</param>
	/// <param name="afterState">: 次の状態</param>
	void TransitionResource(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& beforState, const D3D12_RESOURCE_STATES& afterState);

public:

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetUAV() { return renderResource_->GetUAV().handleGPU; }

	AOENGINE::DxResource* GetResource() { return renderResource_; }

	const ImVec2 GetAvailSize() const { return availSize_; }
	const ImVec2 GetImagePos() const { return imagePos_; }

private:

	
	AOENGINE::DxResource* renderResource_;

	ImVec2 availSize_;
	ImVec2 imagePos_;
};


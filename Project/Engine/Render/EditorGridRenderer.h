#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE {

/// <summary>
/// Editor用の無限グリッドを専用シェーダーで描画する。
/// </summary>
class EditorGridRenderer {
public:
	EditorGridRenderer() = default;
	~EditorGridRenderer();
	EditorGridRenderer(const EditorGridRenderer&) = delete;
	EditorGridRenderer& operator=(const EditorGridRenderer&) = delete;

	void Init(ID3D12Device* device);
	void Finalize();

	void Draw(
		ID3D12GraphicsCommandList* commandList,
		const Math::Matrix4x4& viewMatrix,
		const Math::Matrix4x4& projectionMatrix,
		bool hasMotionVectorTarget);

private:
	struct alignas(16) GridShaderParameter {
		Math::Matrix4x4 inverseViewProjection;
		Math::Matrix4x4 viewProjection;
		Math::Vector3 cameraPosition;
		float baseGridSize;
		float fadeStart;
		float fadeEnd;
		float padding[2];
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> parameterResource_;
	GridShaderParameter* parameter_ = nullptr;
};

} // namespace AOENGINE

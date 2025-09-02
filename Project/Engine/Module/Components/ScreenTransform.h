#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MathStructures.h"

/// <summary>
/// 2d用のTransformをGPUに送るクラス
/// </summary>
class ScreenTransform {
public:

	struct TransformData {
		Matrix4x4 wvp;
	};

public:

	ScreenTransform() = default;
	~ScreenTransform();

	void Init(ID3D12Device* _pDevice);

	void Update(const Matrix4x4& _correctionMat, const Matrix4x4& _projMat);

	void BindCommand(ID3D12GraphicsCommandList* _cmd, uint32_t index);

public:

	void SetTranslate(const Vector2& centerPos) { transform_.translate.x = centerPos.x, transform_.translate.y = centerPos.y, 0.0f; }
	void SetScale(const Vector2 scale) { transform_.scale.x = scale.x, transform_.scale.y = scale.y, transform_.scale.z = 1.0f; }
	void SetRotate(float rotate) { transform_.rotate.z = rotate; }

	const Vector2 GetTranslate() const { return Vector2{ transform_.translate.x, transform_.translate.y }; }
	const Vector2 GetScale() const { return Vector2(transform_.scale.x, transform_.scale.y); }
	const float GetRotate() const { return transform_.rotate.z; }

private:

	ComPtr<ID3D12Resource> transformBuffer_;
	TransformData* transformData_;

	SRT transform_;

};


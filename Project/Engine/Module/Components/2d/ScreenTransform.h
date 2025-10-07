#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "imgui.h"

/// <summary>
/// 2d用のTransformをGPUに送るクラス
/// </summary>
class ScreenTransform {
public:

	struct TransformData {
		Matrix4x4 wvp;
	};

public:

	ScreenTransform();
	~ScreenTransform();

	void Init(ID3D12Device* _pDevice);

	void Update(const Matrix4x4& _projection);

	void BindCommand(ID3D12GraphicsCommandList* _cmd, uint32_t index);

	void Debug_Gui();

	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos);

public:

	void SetSRT(const SRT& _srt) { transform_ = _srt; }
	const SRT& GetSRT() const { return transform_; }

	void SetTranslateX(float x) { transform_.translate.x = x; }
	void SetTranslateY(float y) { transform_.translate.y = y; }
	void SetTranslateZ(float z) { transform_.translate.z = z; }

	void SetTranslate(const Vector2& centerPos) { transform_.translate.x = centerPos.x, transform_.translate.y = centerPos.y, transform_.translate.z = 1.0f; };
	void SetScale(const Vector2 scale) { transform_.scale.x = scale.x, transform_.scale.y = scale.y, transform_.scale.z = 1.0f; }
	void SetRotateZ(float rotate) { transform_.rotate.z = rotate; }

	const Vector2 GetTranslate() const { return Vector2{ transform_.translate.x, transform_.translate.y }; }
	const Vector2 GetScale() const { return Vector2(transform_.scale.x, transform_.scale.y); }
	const float GetRotateZ() const { return transform_.rotate.z; }
	const Vector3 GetRotate() const { return transform_.rotate; }

	const SRT& GetTransform() const { return transform_; }
	void SetTransform(const SRT& transform) { transform_ = transform; }


private:

	ComPtr<ID3D12Resource> transformBuffer_;
	TransformData* transformData_;

	SRT transform_;
	Matrix4x4 screenMat_;

	int id_;
	static int nextId_;

};


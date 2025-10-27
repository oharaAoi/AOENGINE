#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "imgui.h"

/// <summary>
/// 2d用のTransformをGPUに送るクラス
/// </summary>
class ScreenTransform {
public:

	/// <summary>
	/// Gpuに送る構造体
	/// </summary>
	struct TransformData {
		Matrix4x4 wvp;
	};

public:

	ScreenTransform();
	~ScreenTransform();

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_pDevice"></param>
	void Init(ID3D12Device* _pDevice);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_projection"></param>
	void Update(const Matrix4x4& _projection);

	/// <summary>
	/// comanndを積む
	/// </summary>
	/// <param name="_cmd"></param>
	/// <param name="index"></param>
	void BindCommand(ID3D12GraphicsCommandList* _cmd, uint32_t index);

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui();

	/// <summary>
	/// Gizumoを表示
	/// </summary>
	/// <param name="windowSize"></param>
	/// <param name="imagePos"></param>
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos);

public:

	void SetParent(const Matrix4x4& _parentMat);

	const Matrix4x4& GetMatrix() const { return screenMat_; }

	void SetSRT(const SRT& _srt) { transform_ = _srt; }
	const SRT& GetSRT() const { return transform_; }

	void SetTranslateX(float x) { transform_.translate.x = x; }
	void SetTranslateY(float y) { transform_.translate.y = y; }
	void SetTranslateZ(float z) { transform_.translate.z = z; }

	void SetTranslate(const Vector2& _centerPos) { transform_.translate.x = _centerPos.x, transform_.translate.y = _centerPos.y, transform_.translate.z = 1.0f; };
	void SetScale(const Vector2 _scale) { transform_.scale.x = _scale.x, transform_.scale.y = _scale.y, transform_.scale.z = 1.0f; }
	void SetRotateZ(float _rotate) { transform_.rotate.z = _rotate; }

	const Vector2 GetTranslate() const { return Vector2{ transform_.translate.x, transform_.translate.y }; }
	const Vector2 GetScale() const { return Vector2(transform_.scale.x, transform_.scale.y); }
	const float GetRotateZ() const { return transform_.rotate.z; }
	const Vector3 GetRotate() const { return transform_.rotate; }

	const SRT& GetTransform() const { return transform_; }
	void SetTransform(const SRT& _transform) { transform_ = _transform; }

private:

	ComPtr<ID3D12Resource> transformBuffer_;
	TransformData* transformData_;

	SRT transform_;
	Matrix4x4 screenMat_;

	const Matrix4x4* parentMat_ = nullptr;

	int id_;
	static int nextId_;

};


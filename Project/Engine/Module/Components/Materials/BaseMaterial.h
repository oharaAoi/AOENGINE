#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MathStructures.h"

class BaseMaterial {
public:

	BaseMaterial() = default;
	virtual ~BaseMaterial() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void SetCommand() = 0;

	virtual void Debug_Gui() = 0;

public:

	void SetUvScale(const Vector3& _scale) { uvTransform_.scale = _scale; }
	void SetUvRotate(const Vector3& _rotate) { uvTransform_.rotate = _rotate; }
	void SetUvTranslate(const Vector3& _translate) { uvTransform_.translate = _translate; }

protected:

	ComPtr<ID3D12Resource> cBuffer_;

	SRT uvTransform_;

};


#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/System/Manager/ImGuiManager.h"


template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

struct WorldTransformData {
	Matrix4x4 matWorld;
	Matrix4x4 worldInverseTranspose;
};

class WorldTransform {
public:

	WorldTransform();
	~WorldTransform();

	void Finalize();
	void Init(ID3D12Device* device);
	void Update(const Matrix4x4& mat = Matrix4x4::MakeUnit());
	void BindCommandList(ID3D12GraphicsCommandList* commandList, UINT index) const;

	void Translate(const Vector3& translate) { translate_ += translate; }

	void MoveVelocity(const Vector3& velocity, float rotationSpeed);

#ifdef _DEBUG
	void Debug_Gui();
	void Debug_Quaternion();
#endif

public:

	void SetParent(const Matrix4x4& parentMat);
	void SetParentTranslate(const Vector3& parentTranslate);
	void SetParentRotate(const Quaternion& parentQuaternion);

	void SetMatrix(const Matrix4x4& mat);
	void SetScale(const Vector3& scale) { scale_ = scale; }
	void SetTranslaion(const Vector3& translate) { translate_ = translate; }
	void SetQuaternion(const Quaternion& quaternion) { rotation_ = quaternion; }

	void SetTranslationX(float x) { translate_.x = x; }
	void SetTranslationY(float y) { translate_.y = y; }
	void SetTranslationZ(float z) { translate_.z = z; }

	void SetSRT(const QuaternionSRT& srt) {
		translate_ = srt.translate;
		rotation_ = srt.rotate;
		scale_ = srt.scale;
	}

	QuaternionSRT& GetSRT() { return transform_; }
	const Vector3 GetScale() const { return scale_; }
	const Vector3& GetTranslation() const { return translate_; }
	const Quaternion& GetQuaternion() const { return rotation_; }
	const Matrix4x4& GetWorldMatrix() const { return worldMat_; }

public:

	Vector3 scale_;
	Quaternion rotation_;
	Vector3 translate_;

	// 一時的に座標を動かしたい時にこの変数に加算する
	// 例) 浮遊させるときに浮遊の移動量をthisに足す
	Vector3 temporaryTranslate_{};

private:

	Quaternion moveQuaternion_;

	QuaternionSRT transform_;
	Matrix4x4 worldMat_;

	const Matrix4x4* parentWorldMat_ = nullptr;
	const Vector3* parentTranslate_ = nullptr;
	const Quaternion* parentRotate_ = nullptr;

	ComPtr<ID3D12Resource> cBuffer_;
	WorldTransformData* data_;

	float test_angle_ = 0;
};


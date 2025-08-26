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
	Matrix4x4 matWorldPrev;
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

	void Translate(const Vector3& translate) { srt_.translate += translate; }

	void MoveVelocity(const Vector3& velocity, float rotationSpeed);

	void LookAt(const Vector3& target, const Vector3& up = CVector3::UP);

	void Debug_Gui();

	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos);
	
public:

	void SetParent(const Matrix4x4& parentMat);
	void SetParentTranslate(const Vector3& parentTranslate);
	void SetParentRotate(const Quaternion& parentQuaternion);

	void SetMatrix(const Matrix4x4& mat);
	void SetScale(const Vector3& scale) { srt_.scale = scale; }
	void SetTranslate(const Vector3& translate) { srt_.translate = translate; }
	void SetRotate(const Quaternion& quaternion) { srt_.rotate = quaternion; }

	void SetTranslationX(float x) { srt_.translate.x = x; }
	void SetTranslationY(float y) { srt_.translate.y = y; }
	void SetTranslationZ(float z) { srt_.translate.z = z; }

	void SetSRT(const QuaternionSRT& srt) {
		srt_ = srt;
	}

	Vector3 GetPos() const { return Vector3(worldMat_.m[3][0], worldMat_.m[3][1], worldMat_.m[3][2]); }

	QuaternionSRT& GetSRT() { return srt_; }
	const Vector3 GetScale() const { return srt_.scale; }
	const Vector3& GetTranslate() const { return srt_.translate; }
	const Quaternion& GetRotate() const { return srt_.rotate; }
	const Matrix4x4& GetWorldMatrix() const { return worldMat_; }
	const Matrix4x4& GetWorldMatrixPrev() const { return data_->matWorldPrev; }

	void SetBillBoard(bool _isBillBoard) { isBillboard_ = _isBillBoard; }

public:

	QuaternionSRT srt_;
	Vector3 preTranslate_;

	// 一時的に座標を動かしたい時にこの変数に加算する
	// 例) 浮遊させるときに浮遊の移動量をthisに足す
	Vector3 temporaryTranslate_{};

private:

	Matrix4x4 worldMat_;
	Vector3 guiEulerDeg_;
	Quaternion moveQuaternion_;

	const Matrix4x4* parentWorldMat_ = nullptr;
	const Vector3* parentTranslate_ = nullptr;
	const Quaternion* parentRotate_ = nullptr;

	ComPtr<ID3D12Resource> cBuffer_;
	WorldTransformData* data_;

	float test_angle_ = 0;

	int id_;
	static int nextId_;

	bool isBillboard_;
};


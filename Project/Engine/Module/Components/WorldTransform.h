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

namespace AOENGINE {

/// <summary>
/// Gpuに送る情報
/// </summary>
struct WorldTransformData {
	Math::Matrix4x4 matWorld;
	Math::Matrix4x4 matWorldPrev;
	Math::Matrix4x4 worldInverseTranspose;
};

/// <summary>
/// 3d空間での座標などを持ったクラス
/// </summary>
class WorldTransform {
public:

	WorldTransform();
	~WorldTransform();

public:

	// 終了
	void Finalize();
	// 初期化
	void Init(ID3D12Device* device);
	// 更新
	void Update(const Math::Matrix4x4& mat = Math::Matrix4x4::MakeUnit());

	void PostUpdate();
	// コマンドを積む
	void BindCommandList(ID3D12GraphicsCommandList* commandList, UINT index) const;

	/// <summary>
	/// 並行移動させる
	/// </summary>
	/// <param name="translate"></param>
	void Translate(const Math::Vector3& translate, float _deltaTime = 1.0f);

	/// <summary>
	/// 移動速度を加算する
	/// </summary>
	/// <param name="velocity"></param>
	/// <param name="rotationSpeed"></param>
	void MoveVelocity(const Math::Vector3& velocity, float rotationSpeed);

	/// <summary>
	/// target方向を向く
	/// </summary>
	/// <param name="target"></param>
	/// <param name="up"></param>
	void LookAt(const Math::Vector3& target, const Math::Vector3& up = CVector3::UP);

	// 編集処理
	void Debug_Gui();

	// gizumo描画
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos);

public:

	void SetParent(const Math::Matrix4x4& parentMat);
	void SetParentTranslate(const Math::Vector3& parentTranslate);
	void SetParentRotate(const Math::Quaternion& parentQuaternion);

	void SetMatrix(const Math::Matrix4x4& mat);
	void SetScale(const Math::Vector3& scale) { srt_.scale = scale; }
	void SetTranslate(const Math::Vector3& translate) { srt_.translate = translate; }
	void SetRotate(const Math::Quaternion& quaternion) { srt_.rotate = quaternion; }

	void SetScaleX(float x) { srt_.scale.x = x; }
	void SetScaleY(float y) { srt_.scale.y = y; }
	void SetScaleZ(float z) { srt_.scale.z = z; }

	void SetTranslationX(float x) { srt_.translate.x = x; }
	void SetTranslationY(float y) { srt_.translate.y = y; }
	void SetTranslationZ(float z) { srt_.translate.z = z; }

	void SetSRT(const Math::QuaternionSRT& srt) {
		srt_ = srt;
	}

	Math::Vector3 GetPos() const { return Math::Vector3(worldMat_.m[3][0], worldMat_.m[3][1], worldMat_.m[3][2]); }

	/// <summary>
	/// offsetを含めた座標を返す
	/// </summary>
	/// <returns></returns>
	Math::Vector3 GetOffsetPos() const { return Math::Vector3(worldMat_.m[3][0], worldMat_.m[3][1], worldMat_.m[3][2]) + offset_; }

	Math::QuaternionSRT& GetSRT() { return srt_; }
	const Math::Vector3 GetScale() const { return srt_.scale; }
	const Math::Vector3& GetTranslate() const { return srt_.translate; }
	const Math::Quaternion& GetRotate() const { return srt_.rotate; }
	const Math::Matrix4x4& GetWorldMatrix() const { return worldMat_; }
	const Math::Matrix4x4& GetWorldMatrixPrev() const { return data_->matWorldPrev; }

	const Math::Vector3 GetPreTranslate() const { return preTranslate_; }
	void SetPreTranslate(const Math::Vector3& preTranslate) { preTranslate_ = preTranslate; }

	const Math::Vector3& GetTemporaryTranslate() const { return temporaryTranslate_; }
	void SetTemporaryTranslate(const Math::Vector3& pos) { temporaryTranslate_ = pos; }

	void SetBillBoard(bool _isBillBoard) { isBillboard_ = _isBillBoard; }

	void SetOffset(const Math::Vector3& _offset) { offset_ = _offset; }

private:

	Math::QuaternionSRT srt_;
	Math::Vector3 preTranslate_;

	// 一時的に座標を動かしたい時にこの変数に加算する
	// 例) 浮遊させるときに浮遊の移動量をthisに足す
	Math::Vector3 temporaryTranslate_{};

	Math::Matrix4x4 worldMat_;
	Math::Vector3 guiEulerDeg_;
	Math::Quaternion moveQuaternion_;

	const Math::Matrix4x4* parentWorldMat_ = nullptr;
	const Math::Vector3* parentTranslate_ = nullptr;
	const Math::Quaternion* parentRotate_ = nullptr;

	ComPtr<ID3D12Resource> cBuffer_;
	AOENGINE::WorldTransformData* data_;

	float test_angle_ = 0;

	int id_;
	static int nextId_;

	bool isBillboard_;

	Math::Vector3 offset_ = CVector3::ZERO;
};

}
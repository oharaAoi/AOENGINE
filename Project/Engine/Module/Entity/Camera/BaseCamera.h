#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/Module/Components/GameObject/SceneObject.h"

/// <summary>
/// BaseとなるCamera
/// </summary>
class BaseCamera :
	public AOENGINE::SceneObject {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		Math::Quaternion rotate = Math::Quaternion::AngleAxis(25.0f * kToRadian, CVector3::RIGHT);
		Math::Vector3 translate = Math::Vector3(0.0f,4.0f,-8.0f);

		Parameter() {
			SetGroupName("Camera");
			SetName("BaseCamera");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("rotate", rotate)
				.Add("translate", translate)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "rotate", rotate);
			Convert::fromJson(jsonData, "translate", translate);
		}

		void Debug_Gui() override {};
	};

public: // member method

	BaseCamera() = default;
	virtual ~BaseCamera() override;

public:

	// 終了
	virtual void Finalize();
	// 初期化
	virtual void Init();
	// 更新
	virtual void Update();

	/// <summary>
	/// このCameraの行列を現在の描画Viewへ適用する。
	/// Updateと描画Viewの選択を分離し、同一フレームで複数Cameraを利用できるようにする。
	/// </summary>
	void ApplyToRender() const;

public:	// accessor method

	const Math::Quaternion& GetRotate() const { return transform_.rotate; }

	const Math::Matrix4x4& GetCameraMatrix() const { return cameraMatrix_; }

	Math::Matrix4x4 GetViewMatrix() const { return viewMatrix_; }
	Math::Matrix4x4 GetProjectionMatrix() const { return projectionMatrix_; }

	const Math::Matrix4x4 GetVpvpMatrix() const;

	const Math::Matrix4x4 GetBillBordMatrix() const { return billBordMat_; }

	const Math::Matrix4x4 GetVPVMatrix() const;
	const Math::Vector3 GetWorldPosition() const;

protected:

	Parameter parameter_;

	Math::QuaternionSRT transform_;

	Math::Matrix4x4 cameraMatrix_;
	Math::Matrix4x4 projectionMatrix_;
	Math::Matrix4x4 viewMatrix_;
	Math::Matrix4x4 previousProjectionMatrix_;
	Math::Matrix4x4 previousViewMatrix_;
	Math::Matrix4x4 viewportMatrix_;

	Math::Matrix4x4 billBordMat_;

	float fovY_ = 0.45f;
	float near_ = 0.1f;
	float far_ = 10000.0f;
};

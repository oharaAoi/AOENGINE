#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
// DirectX
#include "Engine/DirectX/Utilities/DirectXUtils.h"
// math
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Manager/ImGuiManager.h"

class BaseLight {
public:

	struct LightViewProjectionData {
		Matrix4x4 view;
		Matrix4x4 projection;
	};

	struct BaseParameter : public IJsonConverter {
		Vector3 lightPos = CVector3::ZERO;
		Vector3 direction = Vector3(0, -1, 0);	
		float fovY = 0.45f;
		float nearClip = 0.1f;
		float farClip = 100.0f;

		BaseParameter() {
			toJsonFunction_ = [this](const std::string& id) {
				return this->ToJson(id);
				};
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("lightPos", lightPos)
				.Add("direction", direction)
				.Add("fovY", fovY)
				.Add("nearClip", nearClip)
				.Add("farClip", farClip)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "lightPos", lightPos);
			fromJson(jsonData, "direction", direction);
			fromJson(jsonData, "fovY", fovY);
			fromJson(jsonData, "nearClip", nearClip);
			fromJson(jsonData, "farClip", farClip);
		}
	};

public:

	BaseLight() = default;
	virtual ~BaseLight() = default;

	virtual void Init(ID3D12Device* device, const size_t& size);

	virtual void Finalize();

	virtual void Update();

	virtual void Draw(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex);

	void BindCommandList(ID3D12GraphicsCommandList* commandList, UINT index) const;

	void CalucViewProjection(const Vector3& pos);

	void EditParameter(const std::string& name);

protected:

	Microsoft::WRL::ComPtr<ID3D12Resource>lightBuffer_;

	Vector3 lightPos_ = Vector3(0, 200, 0);
	Vector3 direction_ = Vector3(0, -1, 0);

	ComPtr<ID3D12Resource> cBuffer_;
	LightViewProjectionData* data_;
	Matrix4x4 lightMatrix_;
	Matrix4x4 viewProjectionMatrix_;

	float fovY_ = 0.45f;
	float near_ = -500.f;
	float far_ = 500.0f;

	BaseParameter baseParameter_;

};


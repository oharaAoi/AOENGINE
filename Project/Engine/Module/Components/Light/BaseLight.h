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

/// <summary>
/// lightの基底クラス
/// </summary>
class BaseLight {
public:

	/// <summary>
	/// Gpuに送る情報
	/// </summary>
	struct LightViewProjectionData {
		Matrix4x4 view;
		Matrix4x4 projection;
	};

	/// <summary>
	/// Lightのパラメータ
	/// </summary>
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

		void Debug_Gui() override;
	};

public:

	BaseLight() = default;
	virtual ~BaseLight() = default;

public:

	// 初期化
	virtual void Init(ID3D12Device* device, const size_t& size);
	// 終了
	virtual void Finalize();
	// 更新
	virtual void Update();
	// 描画処理
	virtual void BindCommand(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex);
	// コマンドを積む
	void ViewBindCommand(ID3D12GraphicsCommandList* commandList, UINT index) const;
	// 透視投影行列の計算
	void CalucViewProjection(const Vector3& pos);
	// パラメータの編集
	void EditParameter(const std::string& name);
	// 外部情報の読み込み
	virtual void LoadData() = 0;

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


#pragma once
#include <functional>
#include "BaseLight.h"
#include "Engine/Lib/Color.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 平行光源
/// </summary>
class DirectionalLight :
	public BaseLight,
	public AttributeGui {
public: // メンバ構造体

	/// <summary>
	/// 平行光源の構造体
	/// </summary>
	struct DirectionalLightData {
		Matrix4x4 viewProjection;
		Color color; // ライトの色
		Vector3 direction; // 方向
		float pad;
		Vector3 eyePos;
		float intensity; // 輝度
		float limPower; // リムライトの強さ
	};

	/// <summary>
	/// 保存のパラメータ
	/// </summary>
	struct Paramter : public IJsonConverter {
		Color color = Color(1,1,1,1);		// ライトの色
		Vector3 direction = Vector3(0,-1,0);	// 方向
		float intensity = 1.0f;	// 輝度
		float limPower = 0.5f;		// リムライトの強さ

		Paramter() {
			toJsonFunction_ = [this](const std::string& id) {
				return this->ToJson(id);
				};
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("color", color)
				.Add("direction", direction)
				.Add("intensity", intensity)
				.Add("limPower", limPower)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "color", color);
			fromJson(jsonData, "direction", direction);
			fromJson(jsonData, "intensity", intensity);
			fromJson(jsonData, "limPower", limPower);
		}

		void Debug_Gui() override;
	};

public:

	DirectionalLight();
	~DirectionalLight();

public:

	// 初期化
	void Init(ID3D12Device* device, const size_t& size) override;
	// 終了
	void Finalize() override;
	// 更新
	void Update() override;
	// コマンドを積む
	void BindCommand(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) override;
	// リセット
	void Reset();

public:

	// 編集処理
	void Debug_Gui() override;

	void SetEyePos(const Vector3& pos) { directionalLightData_->eyePos = pos; }

	void SetIntensity(float _intensity) { directionalLightData_->intensity = _intensity; }

private:

	DirectionalLightData* directionalLightData_;

	Paramter parameter_;

	bool isActive_;
};
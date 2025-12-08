#pragma once
#include "BaseLight.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AOENGINE {

/// <summary>
/// 集中光源
/// </summary>
class SpotLight :
	public BaseLight,
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// 集中光源の構造体
	/// </summary>
	struct SpotLightData {
		Math::Matrix4x4 viewProjection;
		AOENGINE::Color color; // ライトの色
		Math::Vector3 position; // ライトの位置
		float pad;
		Math::Vector3 eyePos;
		float intensity; // 輝度
		Math::Vector3 direction; // 方向
		float distance;
		float decay;	// 減衰
		float cosAngle; // スポットライトの余弦
		float cosFalloffStart;
		float padding[1];
	};

	/// <summary>
	/// 保存のパラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		AOENGINE::Color color = AOENGINE::Color::white;		// ライトの色
		Math::Vector3 position = Math::Vector3(0, 1, 0);
		float intensity = 1.0f;
		Math::Vector3 direction = Math::Vector3(0, -1, 0);
		float distance = 5.0f;
		float decay = 2.0f;
		float cosAngle = std::cos(std::numbers::pi_v<float>);
		float cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("color", color)
				.Add("position", position)
				.Add("intensity", intensity)
				.Add("direction", direction)
				.Add("distance", distance)
				.Add("decay", decay)
				.Add("cosAngle", cosAngle)
				.Add("cosFalloffStart", cosFalloffStart)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "color", color);
			Convert::fromJson(jsonData, "position", position);
			Convert::fromJson(jsonData, "intensity", intensity);
			Convert::fromJson(jsonData, "direction", direction);
			Convert::fromJson(jsonData, "decay", decay);
			Convert::fromJson(jsonData, "cosAngle", cosAngle);
			Convert::fromJson(jsonData, "cosFalloffStart", cosFalloffStart);
		}

		void Debug_Gui() override;
	};

public:

	SpotLight();
	~SpotLight();

public:

	// 初期化
	void Init(ID3D12Device* device, const size_t& size) override;
	// 終了
	void Finalize() override;
	// 更新
	void Update() override;
	// コマンドを積む
	void BindCommand(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) override;
	// 編集
	void Debug_Gui();
	// 外部情報の読み込み
	void LoadData() override;

public:


	void SetEyePos(const Math::Vector3& pos) { spotLightData_->eyePos = pos; }

	void SetIntensity(float _intensity) { spotLightData_->intensity = _intensity; }

private:

	SpotLightData* spotLightData_ = nullptr;

	Parameter parameter_;

	float cosDegree_;
	float falloffDegree_;

};

}
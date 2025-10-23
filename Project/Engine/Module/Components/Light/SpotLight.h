#pragma once
#include "BaseLight.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 一部の光源
/// </summary>
class SpotLight :
	public BaseLight,
	public AttributeGui {
public:

	struct SpotLightData {
		Matrix4x4 viewProjection;
		Color color; // ライトの色
		Vector3 position; // ライトの位置
		float pad;
		Vector3 eyePos;
		float intensity; // 輝度
		Vector3 direction; // 方向
		float distance;
		float decay;	// 減衰
		float cosAngle; // スポットライトの余弦
		float cosFalloffStart;
		float padding[1];
	};

	struct Parameter : public IJsonConverter {
		Color color = Color::white;		// ライトの色
		Vector3 position = Vector3(0, 1, 0);
		float intensity = 1.0f; 
		Vector3 direction = Vector3(0, -1, 0);
		float distance = 5.0f;
		float decay = 2.0f;
		float cosAngle = std::cos(std::numbers::pi_v<float>);
		float cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
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
			fromJson(jsonData, "color", color);
			fromJson(jsonData, "position", position);
			fromJson(jsonData, "intensity", intensity);
			fromJson(jsonData, "direction", direction);
			fromJson(jsonData, "decay", decay);
			fromJson(jsonData, "cosAngle", cosAngle);
			fromJson(jsonData, "cosFalloffStart", cosFalloffStart);
		}

		void Debug_Gui() override;
	};

public :

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
	void Draw(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) override;
	// 編集
	void Debug_Gui();

public:


	void SetEyePos(const Vector3& pos) { spotLightData_->eyePos = pos; }

	void SetIntensity(float _intensity) { spotLightData_->intensity = _intensity; }

private:

	SpotLightData* spotLightData_ = nullptr;

	Parameter parameter_;

	float cosDegree_;
	float falloffDegree_;
	
};


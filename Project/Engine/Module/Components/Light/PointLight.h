#pragma once
#include "BaseLight.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 点光源
/// </summary>
class PointLight :
	public BaseLight,
	public AttributeGui {
public: // メンバ構造体

	/// <summary>
	/// 点光源の構造体
	/// </summary>
	struct PointLightData {
		Matrix4x4 viewProjection;
		Color color; // ライトの色
		Vector3 position; // ライトの位置
		float pad;
		Vector3 eyePos;
		float intensity; // 輝度
		float radius;	// 最大距離
		float decay;	// 減衰率
		float padding[2];
	};

	/// <summary>
	/// 保存パラメータ
	/// </summary>
	struct Paramter : public IJsonConverter {
		Color color = Color(1,1,1,1);		// ライトの色
		Vector3 position = Vector3(0,1,0);	// ライトの位置
		float intensity = 1.0f;	// 輝度
		float radius = 5.0f;		// 範囲
		float decay = 0.5f;		// 減衰率

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("color", color)
				.Add("position", position)
				.Add("intensity", intensity)
				.Add("radius", radius)
				.Add("decay", decay)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "color", color);
			fromJson(jsonData, "position", position);
			fromJson(jsonData, "intensity", intensity);
			fromJson(jsonData, "radius", radius);
			fromJson(jsonData, "decay", decay);
		}

		void Debug_Gui() override;
	};

public:

	PointLight();
	~PointLight();

public:

	// 初期化
	void Init(ID3D12Device* device, const size_t& size) override;
	// 終了
	void Finalize() override;
	// 更新
	void Update() override;
	// コマンドを積む
	void BindCommand(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) override;
	// 編集処理
	void Debug_Gui();
	// 外部情報の読み込み
	void LoadData() override;

public:

	void SetEyePos(const Vector3& pos) { pointLightData_->eyePos = pos; }

	void SetIntensity(float _intensity) { pointLightData_->intensity = _intensity; }

private:

	PointLightData* pointLightData_ = nullptr;

	Paramter parameter_;
};


#pragma once
#include <memory>
// Engine
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Engine/DirectX/Resource/DxResource.h"

/// <summary>
/// アーマー単体のクラス
/// </summary>
class PulseArmor :
	public AOENGINE::ISceneObject {
public:

	// Dissolveに必要な構造体
	struct DissolveSetting {
		Math::Matrix4x4 uvTransform[3];
		AOENGINE::Color color;
		AOENGINE::Color edgeColor;
		float threshold = 0.5f;
	};

	struct ArmorParameter : public AOENGINE::IJsonConverter {
		float durability = 100.0f;
		Math::Vector3 scale = CVector3::UNIT;
		Math::Vector3 localTranslate = CVector3::ZERO;
		AOENGINE::Color color = AOENGINE::Color::white;
		AOENGINE::Color edgeColor = AOENGINE::Color::white;

		Math::SRT uvTransform;

		std::string baseTexture = "white.png";
		std::string noiseTexture1;
		std::string noiseTexture2;
		std::string noiseTexture3;

		float minThreshold;
		float maxThreshold;

		ArmorParameter() { SetName("BossArmorParameter"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("durability", durability)
				.Add("scale", scale)
				.Add("localTranslate", localTranslate)
				.Add("color", color)
				.Add("edgeColor", edgeColor)
				.Add("uvScale", uvTransform.scale)
				.Add("uvRotate", uvTransform.rotate)
				.Add("uvTranslate", uvTransform.translate)
				.Add("baseTexture", baseTexture)
				.Add("noiseTexture1", noiseTexture1)
				.Add("noiseTexture2", noiseTexture2)
				.Add("noiseTexture3", noiseTexture3)
				.Add("minThreshold", minThreshold)
				.Add("maxThreshold", maxThreshold)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "durability", durability);
			fromJson(jsonData, "scale", scale);
			fromJson(jsonData, "localTranslate", localTranslate);
			fromJson(jsonData, "color", color);			fromJson(jsonData, "edgeColor", edgeColor);
			fromJson(jsonData, "uvScale", uvTransform.scale);
			fromJson(jsonData, "uvRotate", uvTransform.rotate);
			fromJson(jsonData, "uvTranslate", uvTransform.translate);
			fromJson(jsonData, "baseTexture", baseTexture);
			fromJson(jsonData, "noiseTexture1", noiseTexture1);
			fromJson(jsonData, "noiseTexture2", noiseTexture2);
			fromJson(jsonData, "noiseTexture3", noiseTexture3);
			fromJson(jsonData, "minThreshold", minThreshold);
			fromJson(jsonData, "maxThreshold", maxThreshold);
		}

		void Debug_Gui() override;
	};

public:

	PulseArmor() = default;
	~PulseArmor();

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 遅れて更新
	void PostUpdate() override {};
	// 描画前関数
	void PreDraw() const override {};
	// 描画
	void Draw() const override;
	// 編集
	void Debug_Gui() override;
	// parameterを設定
	void SetParameter();
	// gizumo操作
	void Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) override {};

public:

	/// <summary>
	/// Armorをセットする
	/// </summary>
	/// <param name="_durability">: 耐久度</param>
	/// <param name="_scale">: scale</param>
	/// <param name="_color">: baseColor</param>
	/// <param name="_edgeColor"> : edgeColor</param>
	/// <param name="_uvSrt"> : uv座標系</param>
	void SetArmor(float _durability, const Math::Vector3& _scale, const AOENGINE::Color& _color, const AOENGINE::Color& _edgeColor, const Math::SRT& _uvSrt);

	/// <summary>
	/// 耐久度を減らす関数
	/// </summary>
	/// <param name="_damage"></param>
	void DamageDurability(float _damage);

	/// <summary>
	/// 破壊されたかの確認
	/// </summary>
	/// <returns></returns>
	bool BreakArmor();

	/// <summary>
	/// Armorの耐久度の割合を返す
	/// </summary>
	/// <returns></returns>
	float ArmorDurability() { return durability_ / initDurability_; }

public:

	void SetIsAlive(bool _isAlive) { isAlive_ = _isAlive; }
	bool GetIsAlive() const { return isAlive_; }

	WorldTransform* GetTransform() { return worldTransform_.get(); }

private:

	// 描画に必要な変数 -----------------------
	SphereGeometry geometry_;

	std::shared_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> worldTransform_ = nullptr;

	// dissolveに必要な変数 -----------------------
	AOENGINE::DxResource* settingBuffer_;
	DissolveSetting* setting_;

	std::string noiseTexture_[3];

	Math::SRT uvSrt_[3];

	// armorに関する変数 -----------------------
	float durability_;	// 耐久度
	float initDurability_;

	bool isAlive_;

	AOENGINE::VectorTween<Math::Vector3> uvMovingTween_[3];

	AOENGINE::VectorTween<float> thresholdTween_;

	ArmorParameter armorParam_;
};
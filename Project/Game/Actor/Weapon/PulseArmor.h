#pragma once
// Engine
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

class PulseArmor :
	public ISceneObject {
public:

	// Dissolveに必要な構造体
	struct DissolveSetting {
		Matrix4x4 uvTransform;
		Vector4 color;
		Vector4 edgeColor;
		float threshold = 0.5f;
	};

public:

	PulseArmor() = default;
	~PulseArmor();

	void Init() override;

	void Update() override;

	void PostUpdate() override {};

	void Draw() const override;

	void Debug_Gui() override;

public:

	/// <summary>
	/// Armorをセットする
	/// </summary>
	/// <param name="_durability">: 耐久度</param>
	/// <param name="_scale">: scale</param>
	/// <param name="_color">: baseColor</param>
	/// <param name="_edgeColor"> : edgeColor</param>
	/// <param name="_uvSrt"> : uv座標系</param>
	void SetArmor(float _durability, const Vector3& _scale, const Vector4& _color, const Vector4& _edgeColor, const SRT& _uvSrt);

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

	bool GetIsAlive() const { return isAlive_; }

	WorldTransform* GetTransform() { return worldTransform_.get(); }

private:

	// 描画に必要な変数 -----------------------
	SphereGeometry geometry_;

	std::shared_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> worldTransform_ = nullptr;


	// dissolveに必要な変数 -----------------------
	std::unique_ptr<DxResource> settingBuffer_;
	DissolveSetting* setting_;

	std::string noiseTexture_;

	SRT uvSrt_;

	// armorに関する変数 -----------------------
	float durability_;	// 耐久度
	float initDurability_;

	bool isAlive_;

	Vector3 uvMovingValue_;
	VectorTween<Vector3> uvMovingTween_;
};


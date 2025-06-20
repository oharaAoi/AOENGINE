#pragma once
// Engine
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

class PulseArmor :
	public AttributeGui {
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

	void Init();

	void Update();

	void Draw() const;

#ifdef _DEBUG
	void Debug_Gui();
#endif

public:

	/// <summary>
	/// Armorをセットする
	/// </summary>
	/// <param name="_durability">: 耐久度</param>
	/// <param name="_scale">: scale</param>
	/// <param name="_color">: baseColor</param>
	/// <param name="_edgeColor"> : edgeColor</param>
	void SetArmor(float _durability, const Vector3& _scale, const Vector4& _color, const Vector4& _edgeColor);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="_uvScale"></param>
	/// <param name="_uvRotate"></param>
	/// <param name="_uvTranslate"></param>
	void SetUvSRT(const Vector3& _uvScale, const Vector3& _uvRotate, const Vector3& _uvTranslate);

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

	bool isAlive_;

	Vector3 uvMovingValue_;
	VectorTween<Vector3> uvMovingTween_;
};


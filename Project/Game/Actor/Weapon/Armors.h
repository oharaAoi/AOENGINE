#pragma once
#include <string>
#include "Game/Actor/Weapon/PulseArmor.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class Armors :
	public AttributeGui {
public:

	Armors() = default;
	~Armors() = default;

	void Init(const std::string& ownerName);

	void Update();

	void Draw() const;

	void Debug_Gui() override;

	void SetArmor();

public:

	void SetParent(const Matrix4x4& mat);

	/// <summary>
	/// Armorをセットする
	/// </summary>
	/// <param name="_durability">: 耐久度</param>
	/// <param name="_scale">: scale</param>
	/// <param name="_color">: baseColor</param>
	/// <param name="_edgeColor"> : edgeColor</param>
	/// <param name="_uvSrt"> : uv座標系</param>
	void SetArmorParam(float _durability, const Vector3& _scale, const Vector4& _color, const Vector4& _edgeColor, const SRT& _uvSrt);

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
	float ArmorDurability();

public:

	void SetIsAlive(bool _isAlive);
	bool GetIsAlive() const { return outArmor_->GetIsAlive(); }

	void SetIsDeploy(bool _isDeploy) { isDeploy_ = _isDeploy; }
	bool GetIsDeploy() const { return isDeploy_; }

private:

	PulseArmor* outArmor_;
	PulseArmor* inArmor_;

	bool isDeploy_;

};


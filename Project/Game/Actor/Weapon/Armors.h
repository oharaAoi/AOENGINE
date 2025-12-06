#pragma once
#include <string>
#include "Game/Actor/Weapon/PulseArmor.h"
#include "Engine/Lib/Color.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"

/// <summary>
/// pulseArmorを管理しているクラス
/// </summary>
class Armors :
	public AOENGINE::AttributeGui {
public:

	Armors() = default;
	~Armors() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="ownerName">: 所有者の名前</param>
	void Init(const std::string& ownerName);

	// 更新
	void Update();
	// 描画
	void Draw() const;
	// 編集
	void Debug_Gui() override;
	// アーマーを装備する
	void SetArmor();

public:

	void SetParent(const Math::Matrix4x4& mat);

	/// <summary>
	/// Armorをセットする
	/// </summary>
	/// <param name="_durability">: 耐久度</param>
	/// <param name="_scale">: scale</param>
	/// <param name="_color">: baseColor</param>
	/// <param name="_edgeColor"> : edgeColor</param>
	/// <param name="_uvSrt"> : uv座標系</param>
	void SetArmorParam(float _durability, const Math::Vector3& _scale, const AOENGINE::Color& _color, const AOENGINE::Color& _edgeColor, const Math::SRT& _uvSrt);

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

	GpuParticleEmitter* chargeEmitter_;

};


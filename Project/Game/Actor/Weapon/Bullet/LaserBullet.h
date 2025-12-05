#pragma once
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Collider/LineCollider.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"
// game
#include "Game/Actor/Base/BaseBullet.h"
#include "Game/Effects/LaserCylinder.h"

/// <summary>
/// レーザーの弾
/// </summary>
class LaserBullet :
	public BaseBullet {
public: // データ構造体

	struct LaserParameter : public IJsonConverter {
		float maxLength = 1000.0f;  // 最大距離
		float lifeTime = 3.0f;
		float fadeTime = 1.0f;
		float shotSeValue = 0.4f;
		std::string shaderGraphPath = "";

		Color cylinderColor;

		LaserParameter() { 
			SetGroupName("Bullet");
			SetName("LaserParameter");
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("maxLength", maxLength)
				.Add("fadeTime", fadeTime)
				.Add("lifeTime", lifeTime)
				.Add("shotSeValue", shotSeValue)
				.Add("shaderGraphPath", shaderGraphPath)
				.Add("cylinderColor", cylinderColor)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "maxLength", maxLength);
			fromJson(jsonData, "fadeTime", fadeTime);
			fromJson(jsonData, "lifeTime", lifeTime);
			fromJson(jsonData, "shotSeValue", shotSeValue);
			fromJson(jsonData, "shaderGraphPath", shaderGraphPath);
			fromJson(jsonData, "cylinderColor", cylinderColor);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	LaserBullet() = default;
	~LaserBullet() override;

public:

	// 初期化
	void Init();
	// 更新
	void Update() override;
	// 編集処理
	void Debug_Gui() override;

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(BaseCollider* _other);

	/// <summary>
	/// 発射した際に行なう初期化処理
	/// </summary>
	/// <param name="_pos"></param>
	/// <param name="_targetPos"></param>
	/// <param name="speed"></param>
	void Reset(const Vector3& _pos, const Vector3& _targetPos, float _speed);

private:

	void Stretch();

	void Fade();

private:

	std::unique_ptr<WorldTransform> parentTransform_;
	std::unique_ptr<LaserCylinder> laserCylinder_;

	LaserParameter param_;

	bool isShot_ = false;
	bool isFade_ = false;
	Vector3 targetPos_;

	LineCollider* lineCollider_ = nullptr;

	Vector3 dire_;

	Timer lifeTimer_;
	Timer fadeTimer_;
	Color fadeColor_;

	BaseParticles* shotEffect_;
	BaseParticles* shotParticle_;

};


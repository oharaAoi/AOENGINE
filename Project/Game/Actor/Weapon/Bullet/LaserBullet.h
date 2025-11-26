#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// レーザーの弾
/// </summary>
class LaserBullet :
	public BaseBullet {
public: // データ構造体

	struct LaserParameter : public IJsonConverter {
		float maxLength = 1000.0f;  // 最大距離

		LaserParameter() { SetName("LaserParameter"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("maxLength", maxLength)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "maxLength", maxLength);
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
	/// 発射した際に行なう初期化処理
	/// </summary>
	/// <param name="_pos"></param>
	/// <param name="_targetPos"></param>
	/// <param name="speed"></param>
	void Reset(const Vector3& _pos, const Vector3& _targetPos, float _speed);

private:

	std::unique_ptr<WorldTransform> parentTransform_;

	LaserParameter param_;

	bool isShot_ = false;
	Vector3 targetPos_;
};


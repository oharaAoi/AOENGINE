#pragma once
#include <memory>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include <Lib/Math/Vector3.h>

/// <summary>
/// レティクル
/// </summary>
class Reticle :
	public AOENGINE::AttributeGui {
public: 

	/// <summary>
	/// 積極性を計算するためのパラメータ
	/// </summary>
	struct ReticleParameter : public AOENGINE::IJsonConverter {
		float offsetY;			// 基本
		
		ReticleParameter() {
			SetGroupName("UI");
			SetName("Reticle");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("offsetY", offsetY)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "offsetY", offsetY);
		}

		void Debug_Gui() override;
	};
public:

	Reticle() = default;
	~Reticle() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// ロックオン
	void LockOn();
	// 編集処理
	void Debug_Gui() override;

	void SetReticlePos(AOENGINE::WorldTransform* targetTransform, const Math::Matrix4x4& vpvpMat);

public:		// accessor method

	Math::Vector3 GetTargetPos() const;

	bool GetLockOn() const { return isLockOn_; }

	void ReleaseLockOn();

	const Math::Vector2& GetPos() const { return reticlePos_; }

private:

	AOENGINE::Sprite* lockOnButton_;
	AOENGINE::Sprite* reticle_;

	bool isLockOn_ = false;
	Math::Vector2 defaultPosition_;

	AOENGINE::WorldTransform* targetTransform_;

	Math::Vector2 reticlePos_;

	ReticleParameter reticleParam_;
};


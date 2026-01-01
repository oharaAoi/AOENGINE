#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"

/// <summary>
/// 攻撃に使用するアーマー
/// </summary>
class AttackArmor :
	public AOENGINE::BaseEntity {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		std::string shaderGraphPath = "";			// shaderGraphのパス
		Math::Vector3 uvScale = CVector3::UNIT;		// uvのスケール
		Math::Vector3 upScale = CVector3::UNIT;		// 目標のスケール
		float upScaleTargetTime;					// スケールを大きくする時間
		float disapperTargetTime;					// 消える時間

		Parameter() {
			SetGroupName("Effect");
			SetName("AttackArmorParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("uvScale", uvScale)
				.Add("upScale", upScale)
				.Add("upScaleTargetTime", upScaleTargetTime)
				.Add("disapperTargetTime", disapperTargetTime)
				.Add("shaderGraphPath", shaderGraphPath)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "uvScale", uvScale);
			Convert::fromJson(jsonData, "upScale", upScale);
			Convert::fromJson(jsonData, "upScaleTargetTime", upScaleTargetTime);
			Convert::fromJson(jsonData, "disapperTargetTime", disapperTargetTime);
			Convert::fromJson(jsonData, "shaderGraphPath", shaderGraphPath);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	AttackArmor() = default;
	virtual ~AttackArmor();

public:

	// 初期化
	void Init(const Math::Vector3& _pos);

	// 更新
	void Update();

	// 編集
	virtual void Debug_Gui() override;

private:

	// スケールアップ
	void ScaleUp();

	// 消える
	void Disapper();

public:

	void SetIsStart(bool _isStart) { isStart_ = _isStart; }
	bool GetIsStart() const { return isStart_; }

private:

	std::unique_ptr<AOENGINE::ShaderGraph> shaderGraph_;

	Parameter parameter_;
	AOENGINE::Timer upScaleTimer_;
	AOENGINE::Timer disapperTimer_;

	bool isStart_ = false;
	bool isDisapper_ = false;

};
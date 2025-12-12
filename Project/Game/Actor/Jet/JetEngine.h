#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/Components/GameObject/GeometryObject.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/Lib/Math/Curve.h"
#include "Engine/Utilities/Timer.h"
#include "Game/Effects/JetEngineBurn.h"

/// <summary>
/// jetのエンジンとなるクラス
/// </summary>
class JetEngine :
	public AOENGINE::BaseEntity {
public: // データ構造体

	struct Parameter : public AOENGINE::IJsonConverter {
		float engineIncline = 10.0f;
		Math::Vector3 burnMoveScale = Math::Vector3(0.4f, 1.0 , 0.4f);
		float burnScaleUpTime = 0.5f;
		Math::Curve burnMoveScaleCurve;

		Parameter() {
			SetGroupName("JetEngine");
			SetName("jetEngine");
		}

		json ToJson(const std::string& id) const override {
			json curveJson = burnMoveScaleCurve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("engineIncline", engineIncline)
				.Add("burnMoveScale", burnMoveScale)
				.Add("burnScaleUpTime", burnScaleUpTime)
				.Add("burnMoveScaleCurve", curveJson)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "engineIncline", engineIncline);
			Convert::fromJson(jsonData, "burnMoveScale", burnMoveScale);
			Convert::fromJson(jsonData, "burnScaleUpTime", burnScaleUpTime);
			burnMoveScaleCurve.FromJson(jsonData, "burnMoveScaleCurve");
		}

		void Debug_Gui();
	};

public: // コンストラクタ

	JetEngine() = default;
	~JetEngine();

public:

	// 終了
	void Finalize();
	// 初期化
	void Init();
	// 更新
	void Update(float diftX);
	// 編集
	void Debug_Gui() override;

	/// <summary>
	/// 止める
	/// </summary>
	void JetIsStop();

	/// <summary>
	/// スタートさせる
	/// </summary>
	void JetIsStart();

	/// <summary>
	/// Boostをonにする
	/// </summary>
	void BoostOn() { isBoostMode_ = true; }

public:	// accessor Method

	void SetIsBoostMode() { isBoostMode_ = !isBoostMode_; }

	bool GetIsBoostMode() const { return isBoostMode_; }

private:

	// Parameter -------------------------------------------------
	bool isBoostMode_;

	std::unique_ptr<AOENGINE::WorldTransform> burnParentTransform_ = nullptr;

	Parameter param_;

	std::unique_ptr<AOENGINE::ShaderGraph> shaderGraph_;

	// effects -------------------------------------------------
	AOENGINE::BaseParticles* burnParticle_;
	
	std::unique_ptr<JetEngineBurn> jetEngineBurn_[2];

	// moveによる炎のScale関連 ------------------------------------
	AOENGINE::Timer burnScaleUpTimer_;
	bool isStop_ = true;
};
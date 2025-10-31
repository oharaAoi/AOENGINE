#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Camera/Animation/ICameraAnimation.h"

class FollowCamera;

/// <summary>
/// Boost時のカメラアニメーション
/// </summary>
class CameraAnimationBoost :
	public ICameraAnimation {
public: // データ構造体

	struct BoostAnimationParam : public IJsonConverter {
		bool isExecute = false;
		bool isApproach = false;
		float timer;
		// editor, dragFloat3, 0.1
		Vector3 offset;
		// editor, dragFloat, 0.1
		float time;

		BoostAnimationParam() {
			SetGroupName("Camera");
			SetName("BoostAnimation");
		};

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("offset", offset)
				.Add("time", time)
				.Build(); 
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "offset", offset);
			fromJson(jsonData, "time", time);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	CameraAnimationBoost() = default;
	~CameraAnimationBoost() override = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 実行処理を呼ぶ
	void CallExecute(bool _isRevers) override;
	// 編集処理
	void Debug_Gui() override;

private:

	/// <summary>
	/// Boost時のカメラの寄り
	/// </summary>
	void BoostAnimation();

private:

	BoostAnimationParam param_;
	Vector3 cameraOffset_;

};


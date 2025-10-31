#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Camera/Animation/ICameraAnimation.h"

class FollowCamera;

/// <summary>
/// カメラのアニメーションを行うクラス
/// </summary>
class CameraAnimationShot :
	public ICameraAnimation {
public:	// 構造体

	struct ShotAnimationParam : public IJsonConverter {
		bool isExecute = false;
		bool isApproach = false;
		float timer;
		// editor, dragFloat, 0.1
		float offsetZ;
		// editor, dragFloat, 0.1
		float time;

		ShotAnimationParam() {
			SetGroupName("Camera");
			SetName("ShotAnimation");
		};

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("offsetZ", offsetZ)
				.Add("time", time)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "offsetZ", offsetZ);
			fromJson(jsonData, "time", time);
		}

		void Debug_Gui() override;
	};

public: // default method

	CameraAnimationShot();
	~CameraAnimationShot();

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 弾を撃つ際のアニメーションを実行する
	void CallExecute(bool _isRevers) override;
	// 編集処理
	void Debug_Gui() override;

private: // private method 

	// 
	void ShotAnimation();

private: // private variable

	ShotAnimationParam shotAnimation_;
	Vector3 offset_;
};


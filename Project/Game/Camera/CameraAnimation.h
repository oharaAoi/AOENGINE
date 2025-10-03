#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Camera/FollowCamera.h"

/// <summary>
/// カメラのアニメーションを行うクラス
/// </summary>
class CameraAnimation {
public:	// 構造体

	struct ShotAnimationParam : public IJsonConverter {
		bool isExcute;
		bool isApproach;
		// editor, dragFloat, 0.1
		float offsetZ;
		// editor, dragFloat, 0.1
		float time;

		ShotAnimationParam() {
			SetGroupName("Camera");
			SetName("FollowCamera");
		};

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("distance", offsetZ)
				.Add("rotateDelta", time)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "offsetZ", offsetZ);
			fromJson(jsonData, "time", time);
		}

		void Debug_Gui() override;
	};

public: // default method

	CameraAnimation() = default;
	~CameraAnimation() = default;

	void Init();
	void Update();

	void ExecuteShotAnimation(bool _isApproach);

private: // private method

	void ShotAnimation();

public: // accessor method

	void SetFollowCamera(FollowCamera* _followCamera) { pFollowCamera_ = _followCamera; }

private: // private variable

	FollowCamera* pFollowCamera_ = nullptr;

	ShotAnimationParam shotAnimation_;

	float timer_;

};


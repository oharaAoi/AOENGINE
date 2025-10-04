#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"

class FollowCamera;

/// <summary>
/// カメラのアニメーションを行うクラス
/// </summary>
class CameraAnimation {
public:	// 構造体

	struct ShotAnimationParam : public IJsonConverter {
		bool isExecute;
		bool isApproach;
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

	CameraAnimation();
	~CameraAnimation();

	void Init();
	void Update();

	void ExecuteShotAnimation(bool _isApproach);

	void Debug_Gui();

	bool GetShotAnimationFinish() const { return shotAnimation_.isExecute; }

private: // private method 

	void ShotAnimation();

public: // accessor method

	void SetFollowCamera(FollowCamera* _followCamera) { pFollowCamera_ = _followCamera; }

private: // private variable

	FollowCamera* pFollowCamera_ = nullptr;

	ShotAnimationParam shotAnimation_;
	Vector3 offset_;
};


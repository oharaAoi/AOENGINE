#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

/// <summary>
/// /Clear後の通知を行ってくれるUI
/// </summary>
class ClearNotificationUI :
	public AttributeGui {
public:

	struct Parameter : public IJsonConverter {
		Vector2 startPos;
		Vector2 endPos;
		float duration;

		Parameter() { 
			SetGroupName("CanvasUI");
			SetName("clearNotificationUI");
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("startPos", startPos)
				.Add("endPos", endPos)
				.Add("duration", duration)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "startPos", startPos);
			fromJson(jsonData, "endPos", endPos);
			fromJson(jsonData, "duration", duration);
		}

		void Debug_Gui() override;
	};

public:

	ClearNotificationUI() = default;
	~ClearNotificationUI() = default;

	void Init();

	void Update(bool isBossBreak);

	void Draw() const;

	void Debug_Gui() override;

public:

	void Reset();

	bool GetIsBreak() const { return isBreak_; }

	bool IsFinish() { return posTween_.GetIsFinish(); }

	Sprite* GetSprite() const { return sprite_.get(); }

private:

	bool isBreak_ = false;

	std::unique_ptr<Sprite> sprite_;
	Parameter param_;

	Vector2 pos_;
	VectorTween<Vector2> posTween_;

	VectorTween<float> alphaTween_;

};


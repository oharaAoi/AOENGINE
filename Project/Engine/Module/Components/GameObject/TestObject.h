#pragma once
#include <memory>
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Module/Components/Collider/MeshCollider.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class TestObject
	: public BaseGameObject {
public:

	struct Test : public IJsonConverter {
		Vector3 pos;
		float lifeTime;
		float speed;
		float hp;
		float test;

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("pos", pos)
				.Add("lifeTime", lifeTime)
				.Add("speed", speed)
				.Add("hp", hp)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "pos", pos);
			fromJson(jsonData, "lifeTime", lifeTime);
			fromJson(jsonData, "speed", speed);
			fromJson(jsonData, "hp", hp);
		}
	};

public:

	TestObject();
	~TestObject();

	void Init() override;
	void Update() override;
	void Draw() const override;

	void OnCollisionEnter([[maybe_unused]] MeshCollider& other);
	void OnCollisionStay([[maybe_unused]] MeshCollider& other);
	void OnCollisionExit([[maybe_unused]] MeshCollider& other);

	void Debug_Gui();

private:

	Matrix4x4 swordMat_;
	std::unique_ptr<BaseGameObject> sword_;

	bool isCollision_;

	bool isLerp_ = false;

	Test test_;

	VectorTween<Vector3> tween_;
};


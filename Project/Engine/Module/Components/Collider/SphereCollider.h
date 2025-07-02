#pragma once
#include "Engine/Module/Components/Collider/ICollider.h"

class SphereCollider
	: public ICollider {
public:

	SphereCollider();
	~SphereCollider();

	void Init(const std::string& categoryName, ColliderShape shape) override;
	void Update(const QuaternionSRT& srt) override;
	void Draw() const override;

	void Debug_Gui() override;

public:

	// ------------ 半径 ------------ // 
	void SetRadius(const float& radius) { std::get<Sphere>(shape_).radius = radius; }
	float GetRadius() const { return std::get<Sphere>(shape_).radius; }

private:



};


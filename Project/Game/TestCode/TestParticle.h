#pragma once
#include <memory>
#include <vector>
#include "Engine/Components/GameObject/GeometryObject.h"

class TestParticle {
public:

	struct ParticleSRT {
		QuaternionSRT transform;
		Vector4 color;
	};

public:

	TestParticle() = default;
	~TestParticle() = default;

	void Init(float distance);

	void Update(const Quaternion& bill);

	void Emit();

private:

	std::string name_;

	std::unique_ptr<GeometryObject> shape_;

	std::vector<ParticleSRT> srtArray_;

};


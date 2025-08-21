#pragma once
#include <vector>
#include "Engine/Lib/Math/MathStructures.h"

enum class MaterialType {
	NORMAL,
	PBR
};

struct ModelMaterialData {
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::string textureFilePath = "white.png";
};
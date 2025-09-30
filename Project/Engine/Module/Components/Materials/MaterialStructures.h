#pragma once
#include <vector>
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MathStructures.h"

enum class MaterialType {
	NORMAL,
	PBR
};

struct ModelMaterialData {
	Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::string textureFilePath = "white.png";
};
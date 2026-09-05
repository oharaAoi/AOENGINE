#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "GradientData.h"

namespace AOENGINE {

class GradientNode : public BaseShaderGraphNode {
public:
	void Init() override;
	void customUpdate() override {}
	void updateGui() override;
	void draw() override;
	nlohmann::json toJson() override;
	void fromJson(const nlohmann::json& json) override;

private:
	GradientData gradient_;
};

}

#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "GradientData.h"

namespace AOENGINE {

class SampleGradientNode : public BaseShaderGraphNode {
public:
	~SampleGradientNode() override;
	void Init() override;
	void customUpdate() override;
	void updateGui() override;
	void draw() override;
	nlohmann::json toJson() override;
	void fromJson(const nlohmann::json& json) override;

private:
	struct GpuKey {
		std::array<float, 4> color;
		float position;
		float padding[3];
	};
	struct Params {
		GpuKey keys[GradientData::kMaxKeys];
		uint32_t keyCount;
		uint32_t channel;
		uint32_t multiplySourceAlpha;
		uint32_t padding;
	};
	static_assert(sizeof(GpuKey) == 32 && sizeof(Params) == 272);

	GraphicsContext* ctx_ = nullptr;
	DxResource* outputResource_ = nullptr;
	ComPtr<ID3D12Resource> buffer_;
	Params* param_ = nullptr;
	int channel_ = 0;
	bool multiplySourceAlpha_ = true;
	bool hasOutput_ = false;
};

}

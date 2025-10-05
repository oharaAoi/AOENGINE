#include "Object3d.hlsli"

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output = (PixelShaderOutput) 0;
	output.color = float4(input.position.z, input.position.z, input.position.z, 1.0f);
	return output;
}
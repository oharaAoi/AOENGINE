#include "Object3d.hlsli"

struct PSOut {
	float depth : SV_Depth;
};

PSOut main(VertexShaderOutput input) {
	PSOut o;
    // 正規化された深度を書く
	o.depth = input.position.z / input.position.w;
	return o;
}
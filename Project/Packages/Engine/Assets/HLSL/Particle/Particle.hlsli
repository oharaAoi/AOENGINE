
struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color : COLOR0;
	float discardValue : DISCARD0;
};

struct CpuParticle {
	float4x4 worldMat;
	float4 color;
	int draw2d;
	float discardValue;
};

struct GpuParticle {
	float4 color;
	float3 scale;
	float3 targetScale;
	float3 rotate;
	float3 pos;
	float3 velocity;
	float3 acceleration;

	float lifeTime;
	float currentTime;
	float damping;
	float gravity;
	
	int emitType;
	int lifeOfScaleDown;
	int lifeOfScaleUp;
	int lifeOfAlpha;
	
	int isDraw2d;
};

struct MaxParticle {
	int maxParticles;
};
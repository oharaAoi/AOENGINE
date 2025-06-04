#include "Shader.h"

Shader::Shader() {
}

Shader::~Shader() {
}

void Shader::Init() {
	// VS/PS
	Load("Engine/Assets/HLSL/Object3d.VS.hlsl",			"Engine/Assets/HLSL/Object3d.PS.hlsl", Shader::Normal);
	Load("Engine/Assets/HLSL/Object3d.VS.hlsl",			"Engine/Assets/HLSL/Object3dEnviroment.PS.hlsl", Shader::NormalEnviroment);
	Load("Engine/Assets/HLSL/Object3d.VS.hlsl",			"Engine/Assets/HLSL/Textureless.PS.hlsl", Shader::TextureLess);
	Load("Engine/Assets/HLSL/Primitive.VS.hlsl",			"Engine/Assets/HLSL/Primitive.PS.hlsl", Shader::Primitive);
	Load("Engine/Assets/HLSL/Object3d.VS.hlsl",			"Engine/Assets/HLSL/Phong.Lighting.hlsl", Shader::Phong);
	Load("Engine/Assets/HLSL/PBR.VS.hlsl",				"Engine/Assets/HLSL/PBR.PS.hlsl", Shader::PBR);
	Load("Engine/Assets/HLSL/Skybox.VS.hlsl",				"Engine/Assets/HLSL/Skybox.PS.hlsl", Shader::Skybox);
	Load("Engine/Assets/HLSL/Sprite.VS.hlsl",			"Engine/Assets/HLSL/Sprite.PS.hlsl", Shader::Sprite);
	Load("Engine/Assets/HLSL/Particle/CpuParticle.VS.hlsl", "Engine/Assets/HLSL/Particle/CpuParticle.PS.hlsl", Shader::Particle);
	Load("Engine/Assets/HLSL/ProcessedScene.VS.hlsl", "Engine/Assets/HLSL/ProcessedScene.PS.hlsl", Shader::ProcessedScene);
	Load("Engine/Assets/HLSL/ProcessedScene.VS.hlsl", "Engine/Assets/HLSL/Grayscale.PS.hlsl", Shader::Grayscale);
	Load("Engine/Assets/HLSL/ProcessedScene.VS.hlsl", "Engine/Assets/HLSL/RadialBlur.PS.hlsl", Shader::RadialBlur);
	Load("Engine/Assets/HLSL/ProcessedScene.VS.hlsl", "Engine/Assets/HLSL/GlitchNoise.PS.hlsl", Shader::GlitchNoise);
	Load("Engine/Assets/HLSL/ProcessedScene.VS.hlsl", "Engine/Assets/HLSL/Vignette.PS.hlsl", Shader::Vignette);

	Load("Engine/Assets/HLSL/Object3d.VS.hlsl", "Engine/Assets/HLSL/ObjectTextureBlend.PS.hlsl", Shader::ObjectTextureBlend);

	// CS
	Load("Engine/Assets/HLSL/CS/GrayScale.CS.hlsl", CsShaderName::GrayScale);
	Load("Engine/Assets/HLSL/CS/HorizontalBlur.CS.hlsl", CsShaderName::HorizontalBlur);
	Load("Engine/Assets/HLSL/CS/VerticalBlur.CS.hlsl", CsShaderName::VerticalBlur);
	Load("Engine/Assets/HLSL/CS/DepthOfField.CS.hlsl", CsShaderName::DepthOfField);
	Load("Engine/Assets/HLSL/CS/Skinning.CS.hlsl", CsShaderName::SkinningCS);
	Load("Engine/Assets/HLSL/CS/Blend.CS.hlsl", CsShaderName::Blend);
	Load("Engine/Assets/HLSL/CS/ResultRender.CS.hlsl", CsShaderName::Result);

	Load("Engine/Assets/HLSL/Particle/GpuParticleInit.CS.hlsl", CsShaderName::GpuParticleInit);
	Load("Engine/Assets/HLSL/Particle/GpuParticleUpdate.CS.hlsl", CsShaderName::GpuParticleUpdate);
	Load("Engine/Assets/HLSL/Particle/GpuParticleEnd.CS.hlsl", CsShaderName::GpuParticleEnd);
	Load("Engine/Assets/HLSL/Particle/EmitParticle.CS.hlsl", CsShaderName::EmitGpuParticle);
}

void Shader::Load(const std::string& vsPath, const std::string& psPath, const ShaderName& type) {
	ShaderData data{};
	data.vsShader = vsPath;
	data.psShader = psPath;

	shaderData_[type] = data;
}

void Shader::Load(const std::string& csPath, const CsShaderName& type) {
	csShaderData_[type] = csPath;
}

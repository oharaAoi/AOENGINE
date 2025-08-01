#pragma once
#include <string>
#include <memory>
#include <list>
#include "Engine/Render/GpuParticleRenderer.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"
#include "Engine/System/ParticleSystem/Field/GpuParticleField.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class GpuParticleManager :
	public AttributeGui {
public:

	GpuParticleManager() = default;
	~GpuParticleManager();
	GpuParticleManager(const GpuParticleManager&) = delete;
	const GpuParticleManager& operator=(const GpuParticleManager&) = delete;

	static GpuParticleManager* GetInstance();

	void Finalize();

	void Init();

	void Update();

	void Draw() const;

	void Debug_Gui() override;

	GpuParticleEmitter* CreateEmitter(const std::string& particlesFile);

	GpuParticleField* CreateField(const std::string& particlesFile);

private:

	void AddEmitter(GpuParticleEmitter* _emitter);

private:

	std::unique_ptr<GpuParticleRenderer> renderer_;

	std::list<std::unique_ptr<GpuParticleEmitter>> emitterList_;

	std::list<std::unique_ptr<GpuParticleField>> fileds_;

};


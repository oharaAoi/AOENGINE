#pragma once
#include <memory>
#include <vector>
#include <list>

#include "Engine/Lib/ParticlesData.h"
#include "Engine/Render/ParticleInstancingRenderer.h"

namespace AOENGINE {

struct ParticleRuntimeState {
	std::shared_ptr<std::list<AOENGINE::ParticleSingle>> particles;
	std::vector<AOENGINE::ParticleInstancingRenderer::ParticleData> forGpuData_;

	bool isAddBlend = false;
	bool anyParticleAlive = false;

	ParticleRuntimeState() {
		particles = std::make_shared<std::list<AOENGINE::ParticleSingle>>();
	}
};

}
#include "ParticleManager.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Render.h"

ParticleManager::~ParticleManager() {
	Finalize();
}

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Finalize() {
	particlesMap_.clear();
	particleRenderer_.reset();
	emitterList_.clear();
}

#ifdef _DEBUG
void ParticleManager::Debug_Gui() {
	
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Init() {
	SetName("ParticleManager");

	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(20000);

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "ParticleManager");
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Update() {
	ParticlesUpdate();



	for (auto & particles : particlesMap_) {
		particleRenderer_->Update(particles.first, particles.second.forGpuData_, particles.second.isAddBlend);
	}
	/*for (auto& emitter : emitterList_) {
		particleRenderer_->Update(emitter->GetName(), emitter->GetData());
	}*/
}

void ParticleManager::ParticlesUpdate() {
	for (auto& particles : particlesMap_) {

		size_t particleNum = particles.second.particles->size();
		particles.second.forGpuData_.resize(particleNum);
		for (uint32_t oi = 0; oi < particleNum; ++oi) {
			particles.second.forGpuData_[oi].color.w = 0.0f;
		}

		size_t index = 0;
		for (auto it = particles.second.particles->begin(); it != particles.second.particles->end();) {
			auto& pr = *it;
			// ---------------------------
			// 生存時間の更新
			// ---------------------------
			pr.lifeTime -= GameTimer::DeltaTime();
			if (pr.lifeTime <= 0.0f) {
				it = particles.second.particles->erase(it); // 削除して次の要素にスキップ
				continue;
			}

			// ---------------------------
			// Parameterの更新
			// ---------------------------
			// 速度を更新
			pr.velocity *= std::powf((1.0f - pr.damping), GameTimer::DeltaTime());

			// 重力を適応
			pr.velocity.y += pr.gravity * GameTimer::DeltaTime();

			// 座標を適応
			pr.translate += pr.velocity * GameTimer::DeltaTime();

			// ---------------------------
			// 状態の更新
			// ---------------------------
			float t = pr.lifeTime / pr.firstLifeTime;
			t = 1.0f - t;
			if (pr.isLifeOfAlpha) {
				pr.color.w = Lerp(1.0f, 0.0f, t);
			}

			if (pr.isLifeOfScale) {
				pr.scale = Vector3::Lerp(pr.firstScale, CVector3::ZERO, t);
			}

			if (pr.isScaleUpScale) {
				float scaleT = pr.lifeTime / pr.firstLifeTime;
				scaleT = 1.0f - scaleT;
				pr.scale = Vector3::Lerp(CVector3::ZERO, pr.upScale, scaleT);
			}

			Matrix4x4 scaleMatrix = pr.scale.MakeScaleMat();
			Matrix4x4 billMatrix = Render::GetCameraRotate().MakeMatrix(); // ← ビルボード行列（カメラからの視線で作る）
			Matrix4x4 zRot = pr.rotate.MakeMatrix();
			Matrix4x4 rotateMatrix = Multiply(zRot, Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), billMatrix));
			Matrix4x4 translateMatrix = pr.translate.MakeTranslateMat();
			Matrix4x4 localWorld = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

			particles.second.forGpuData_[index].worldMat = localWorld;
			particles.second.forGpuData_[index].color = pr.color;

			particles.second.isAddBlend = pr.isAddBlend;

			// ---------------------------
			// NextFrameのための更新
			// ---------------------------
			++index;
			++it;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 全体更新後処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::PostUpdate() {
	particleRenderer_->PostUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Draw() const {
	particleRenderer_->Draw(GraphicsContext::GetInstance()->GetCommandList());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

BaseParticles* ParticleManager::CrateParticle(const std::string& particlesFile) {
	auto& newParticles = emitterList_.emplace_back(std::make_unique<BaseParticles>());
	newParticles->Init(particlesFile);
	newParticles->SetShareMaterial(
		particleRenderer_->AddParticle(newParticles->GetName(),
									   newParticles->GetGeometryObject()->GetMesh(),
									   newParticles->GetIsAddBlend())
	);

	newParticles->GetShareMaterial()->SetUseTexture(newParticles->GetUseTexture());

	if (!particlesMap_.contains(particlesFile)) {
		particlesMap_.emplace(particlesFile, ParticleManager::ParticlesData());
		AddChild(newParticles.get());
	} 
	newParticles->SetParticlesList(particlesMap_[particlesFile].particles);
	particlesMap_[particlesFile].isAddBlend = newParticles->GetIsAddBlend();

	return newParticles.get();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::DeleteParticles(BaseParticles* ptr) {
	for (auto it = emitterList_.begin(); it != emitterList_.end(); ) {
		if (it->get() == ptr) {
			DeleteChild(it->get()); // 削除時の追加処理
			it = emitterList_.erase(it); // 要素の削除とイテレータ更新
		} else {
			++it;
		}
	}
}

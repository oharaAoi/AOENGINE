#include "ParticleManager.h"
#include "Engine/Engine.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
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
	emitterList_.clear();
	particleRenderer_ = nullptr;
}

void ParticleManager::Debug_Gui() {
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Init() {
	SetName("ParticleManager");

	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(15000);

	EditorWindows::AddObjectWindow(this, "ParticleManager");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Update() {
	this->SetView(Render::GetViewProjectionMat(), Matrix4x4::MakeUnit());

	ParticlesUpdate();

	for (auto& emitter : emitterList_) {
		if (emitter->GetChangeMesh()) {
			emitter->ChangeMesh();
			particleRenderer_->ChangeMesh(emitter->GetName(), emitter->GetMesh());
		}
		emitter->Update();
	}

	for (auto & particles : particlesMap_) {
		particleRenderer_->Update(particles.first, particles.second.forGpuData_, particles.second.isAddBlend);
	}

	PostUpdate();
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

			if (pr.stretchBillboard) {
				Vector3 velocityDir = pr.velocity.Normalize();
				float stretchLength = pr.velocity.Length() * pr.stretchScaleFactor;
				pr.scale.z = pr.firstScale.z * stretchLength;
			}


			Matrix4x4 scaleMatrix = pr.scale.MakeScaleMat();
			Matrix4x4 rotateMatrix;
			if (pr.isBillBord) {
				Matrix4x4 billMatrix = Render::GetCameraRotate().MakeMatrix();
				Matrix4x4 zRot = pr.rotate.MakeMatrix();
				rotateMatrix = Multiply(zRot, Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), billMatrix));
			} else {
				Matrix4x4 billMatrix = Matrix4x4::MakeUnit();
				rotateMatrix = pr.rotate.MakeMatrix();
			}
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
	std::string textureName = newParticles->GetUseTexture();
	newParticles->SetShareMaterial(
		particleRenderer_->AddParticle(newParticles->GetName(),
									   textureName,
									   newParticles->GetMesh(),
									   newParticles->GetIsAddBlend())
	);

	newParticles->GetShareMaterial()->SetAlbedoTexture(newParticles->GetUseTexture());

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

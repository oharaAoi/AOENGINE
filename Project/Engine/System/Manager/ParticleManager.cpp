#include "ParticleManager.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Render.h"

using namespace AOENGINE;

ParticleManager::~ParticleManager() {
	Finalize();
}

AOENGINE::ParticleManager* AOENGINE::ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Finalize() {
	particlesMap_.clear();
	emitterList_.clear();
	particleRenderer_ = nullptr;
	ClearChild();
}

void ParticleManager::Debug_Gui() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Init() {
	SetName("ParticleManager");

	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(51600 * 2);

	AOENGINE::EditorWindows::AddObjectWindow(this, "ParticleManager");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Update() {
	this->SetView(AOENGINE::Render::GetViewProjectionMat(), AOENGINE::Render::GetProjection2D(), Math::Matrix4x4::MakeUnit());

	// particleの更新
	ParticlesUpdate();

	// emitterの更新
	for (auto& emitter : emitterList_) {
		if (emitter->GetChangeMesh()) {
			emitter->ChangeMesh();
			particleRenderer_->ChangeMesh(emitter->GetName(), emitter->GetMesh());
		}
		emitter->Update();
	}

	// renderの更新
	for (auto& particles : particlesMap_) {
		particleRenderer_->Update(particles.first, particles.second.forGpuData_, particles.second.anyParticleAlive, particles.second.isAddBlend);
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
		bool anyParticleAlive = false;
		for (auto it = particles.second.particles->begin(); it != particles.second.particles->end();) {
			auto& pr = *it;
			// ---------------------------
			// 生存時間の更新
			// ---------------------------

			pr.currentTime += AOENGINE::GameTimer::DeltaTime();
			if (pr.currentTime >= pr.lifeTime) {
				it = particles.second.particles->erase(it); // 削除して次の要素にスキップ
				continue;
			}

			if (pr.lifeTime <= 0.f) {
				continue;
			}

			anyParticleAlive = true;

			// ---------------------------
			// Parameterの更新
			// ---------------------------

			if (pr.isCenterFor) {
				if (Length(pr.emitterCenter - pr.translate) < 0.1f) {
					pr.velocity = CVector3::ZERO;
				}
			}

			// 速度を更新
			pr.velocity *= std::powf((1.0f - pr.damping), AOENGINE::GameTimer::DeltaTime());

			// 重力を適応
			pr.velocity.y += pr.gravity * AOENGINE::GameTimer::DeltaTime();

			// 座標を適応
			pr.translate += pr.velocity * AOENGINE::GameTimer::DeltaTime();

			// ---------------------------
			// 状態の更新
			// ---------------------------
			float t = pr.currentTime / pr.lifeTime;
			if (pr.isColorAnimation) {
				pr.color = AOENGINE::Color::Lerp(pr.preColor, pr.postColor, t);
			}

			if (pr.isLifeOfAlpha) {
				pr.color.a = Lerp(1.0f, 0.0f, t);
			}

			if (pr.isLifeOfScale) {
				pr.scale = Math::Vector3::Lerp(pr.lifeOfMinScale, pr.lifeOfMaxScale, t);
			}

			if (pr.isScaleUpScale) {
				pr.scale = Math::Vector3::Lerp(CVector3::ZERO, pr.upScale, t);
			}

			if (pr.isFadeInOut) {
				if (pr.currentTime <= pr.fadeInTime) {
					float alphaT = pr.currentTime / pr.fadeInTime;
					pr.color.a = Lerp(0.0f, pr.initAlpha_, alphaT);
				}

				if ((pr.lifeTime - pr.currentTime) <= pr.fadeOutTime) {
					float alphaT = (pr.fadeOutTime - (pr.lifeTime - pr.currentTime)) / pr.fadeOutTime;
					pr.color.a = Lerp(pr.initAlpha_, 0.0f, alphaT);
				}
			}

			// 閾値を更新
			if (pr.isLerpDiscardValue) {
				pr.discardValue = std::lerp(pr.startDiscard, pr.endDiscard, t);
			}

			Math::Matrix4x4 scaleMatrix = pr.scale.MakeScaleMat();
			Math::Matrix4x4 rotateMatrix;
			if (pr.isBillBord) {
				Math::Matrix4x4 billMatrix = AOENGINE::Render::GetCameraRotate().MakeMatrix();
				Math::Matrix4x4 zRot = pr.rotate.MakeMatrix();
				rotateMatrix = Multiply(zRot, Multiply(Math::Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), billMatrix));
			} else {
				Math::Matrix4x4 billMatrix = Math::Matrix4x4::MakeUnit();
				rotateMatrix = pr.rotate.MakeMatrix();
			}
			if (pr.isDraw2d) {
				pr.translate.z = 0.0f;
			}
			Math::Matrix4x4 translateMatrix = pr.translate.MakeTranslateMat();
			Math::Matrix4x4 localWorld = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

			if (pr.isTextureAnimation) {
				int totalFrames = (int)(pr.tileSize.x * pr.tileSize.y);
				int frame = (int)(t * (totalFrames - 1));

				Math::Vector2 tileSize = { 1.0f / pr.tileSize.x, 1.0f / pr.tileSize.y };

				int frameX = frame % (int)pr.tileSize.x;      // 列
				int frameY = frame / (int)pr.tileSize.x;      // 行

				Math::Vector3 uvTranslate = { frameX * tileSize.x, frameY * tileSize.y, 0.0f };

				Math::Matrix4x4 scaleMat = Math::Vector3(tileSize.x, tileSize.y, 0.0f).MakeScaleMat();
				Math::Matrix4x4 transMat = uvTranslate.MakeTranslateMat();
				pr.uvMat = Multiply(scaleMat, transMat);
			} else {
				pr.uvMat = Math::Matrix4x4::MakeUnit();
			}

			// ---------------------------
			// パラメーターの更新
			// ---------------------------
			particles.second.forGpuData_[index].uvTransform = pr.uvMat;
			particles.second.forGpuData_[index].worldMat = localWorld;
			particles.second.forGpuData_[index].color = pr.color;
			particles.second.forGpuData_[index].draw2d = pr.isDraw2d;
			particles.second.forGpuData_[index].discardValue = pr.discardValue;
			particles.second.forGpuData_[index].velocity = pr.velocity;
			particles.second.forGpuData_[index].cameraPos = AOENGINE::Render::GetEyePos();
			particles.second.forGpuData_[index].isStretch = pr.isStretch;

			particles.second.isAddBlend = pr.isAddBlend;
			particles.second.isAddBlend = pr.isAddBlend;

			// ---------------------------
			// NextFrameのための更新
			// ---------------------------
			++index;
			++it;
		}

		particles.second.anyParticleAlive = anyParticleAlive;
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
	particleRenderer_->Draw(AOENGINE::GraphicsContext::GetInstance()->GetCommandList());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::BaseParticles* ParticleManager::CrateParticle(const std::string& particlesFile) {
	auto& newParticles = emitterList_.emplace_back(std::make_unique<AOENGINE::BaseParticles>());
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

void ParticleManager::DeleteParticles(AOENGINE::BaseParticles* ptr) {
	for (auto it = emitterList_.begin(); it != emitterList_.end(); ) {
		if (it->get() == ptr) {
			DeleteChild(it->get()); // 削除時の追加処理
			it = emitterList_.erase(it); // 要素の削除とイテレータ更新
		} else {
			++it;
		}
	}
}

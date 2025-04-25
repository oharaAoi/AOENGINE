#include "TestParticle.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/GameTimer.h"

void TestParticle::Init() {
	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();

	ParticleManager::GetInstance()->AddParticle(name_, shape_->GetMesh(), shape_->GetMaterial(), true);

	shape_->GetMaterial()->SetUseTexture("circle.png");

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, name_);
#endif // _DEBUG
}

void TestParticle::Update(const Quaternion& bill) {
	//Emit(emitter_.translate);

	std::vector<ParticleInstancingRenderer::ParticleData> data;
	size_t particleNum = particleArray_.size();
	data.resize(particleNum);

	size_t index = 0;
	for (auto it = particleArray_.begin(); it != particleArray_.end();) {
		auto& pr = *it;
		// ---------------------------
		// 生存時間の更新
		// ---------------------------
		pr.lifeTime -= GameTimer::DeltaTime();
		if (pr.lifeTime <= 0.0f) {
			it = particleArray_.erase(it); // 削除して次の要素にスキップ
			continue;
		}

		// ---------------------------
		// Parameterの更新
		// ---------------------------
		pr.translate += pr.velocity * GameTimer::DeltaTime();

		// ---------------------------
		// Rendererに送る情報を更新
		// ---------------------------
		Matrix4x4 scaleMatrix = pr.scale.MakeScaleMat();
		Matrix4x4 rotateMatrix = Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), bill.MakeMatrix());
		Matrix4x4 translateMatrix = pr.translate.MakeTranslateMat();

		data[index].worldMat = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
		data[index].color = { 1,1,1,1 };

		// ---------------------------
		// NextFrameのための更新
		// ---------------------------
		++index;
		++it;
	}
	ParticleManager::GetInstance()->Update(name_, data);
}

void TestParticle::Emit(const Vector3& pos) {
	for (uint32_t oi = 0; oi < emitter_.count; ++oi) {
		auto& newParticle = particleArray_.emplace_back();

		newParticle.scale = RandomVector3(emitter_.minScale, emitter_.maxScale);
		newParticle.rotate = Quaternion();
		newParticle.translate = pos;
		newParticle.color = emitter_.color;
		if (emitter_.shape == 0) {
			newParticle.velocity = RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT).Normalize() * emitter_.speed;
		} else if (emitter_.shape == 1) {
			newParticle.velocity = emitter_.direction.Normalize() * emitter_.speed;
		}
		newParticle.lifeTime = emitter_.lifeTime;
		newParticle.currentTime = 0.0f;
		newParticle.damping = emitter_.dampig;
		newParticle.gravity = emitter_.gravity;
	}
}

#ifdef _DEBUG
void TestParticle::Debug_Gui() {
	emitter_.Attribute_Gui();

	shape_->GetMaterial()->ImGuiDraw();

	if (ImGui::Button("emit")) {
		Emit(emitter_.translate);
	}
}
#endif
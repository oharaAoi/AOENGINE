#include "TestParticle.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Lib/Math/MyRandom.h"

void TestParticle::Init(float distance) {
	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();

	particleArray_.resize(100);
	for (uint32_t oi = 0; oi < 100; ++oi) {
		particleArray_[oi].scale = CVector3::UNIT;
		particleArray_[oi].rotate = Quaternion();
		particleArray_[oi].translate = { 0.5f * oi, 0.0f, distance };

		particleArray_[oi].color = { 1,1,1,1 };
	}

	ParticleManager::GetInstance()->AddParticle(name_, shape_->GetMesh(), shape_->GetMaterial());

	shape_->GetMaterial()->SetUseTexture("circle.png");

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, name_);
#endif // _DEBUG
}

void TestParticle::Update(const Quaternion& bill) {
	std::vector<ParticleInstancingRenderer::ParticleData> data;
	data.resize(100);
	for (uint32_t oi = 0; oi < 100; ++oi) {
		Matrix4x4 scaleMatrix = particleArray_[oi].scale.MakeScaleMat();
		Matrix4x4 rotateMatrix = Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), bill.MakeMatrix());
		Matrix4x4 translateMatrix = particleArray_[oi].translate.MakeTranslateMat();

		data[oi].worldMat = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
		data[oi].color = { 1,1,1,1 };
	}
	ParticleManager::GetInstance()->Update(name_, data);
}

void TestParticle::Emit(const Vector3& pos) {
	auto& newParticle = particleArray_.emplace_back();
	
	newParticle.scale = RandomVector3(emitter_.minScale, emitter_.maxScale);
	newParticle.rotate = Quaternion();
	newParticle.translate = pos;
	newParticle.color = emitter_.color;
	//newParticle.

}

#ifdef _DEBUG
void TestParticle::Debug_Gui() {
	emitter_.Attribute_Gui();
}
#endif
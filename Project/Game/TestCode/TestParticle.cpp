#include "TestParticle.h"
#include "Engine/System/Manager/ParticleManager.h"

void TestParticle::Init() {
	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();

	srtArray_.resize(100);
	for (uint32_t oi = 0; oi < 100; ++oi) {
		srtArray_[oi].transform.scale = CVector3::UNIT;
		srtArray_[oi].transform.rotate = Quaternion();
		srtArray_[oi].transform.translate = { 0.5f * oi, 0.0f, 0.0f };

		srtArray_[oi].color = { 1,1,1,1 };
	}

	ParticleManager::GetInstance()->AddParticle("plane", shape_->GetMesh(), shape_->GetMaterial());
}

void TestParticle::Update(Quaternion bill) {
	std::vector<ParticleInstancingRenderer::ParticleData> data;
	data.resize(100);
	for (uint32_t oi = 0; oi < 100; ++oi) {
		Matrix4x4 scaleMatrix = srtArray_[oi].transform.scale.MakeScaleMat();
		Matrix4x4 rotateMatrix = Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), bill.MakeMatrix());
		Matrix4x4 translateMatrix = srtArray_[oi].transform.translate.MakeTranslateMat();

		data[oi].worldMat = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
		data[oi].color = { 1,1,1,1 };
	}
	ParticleManager::GetInstance()->Update("plane", data);
}

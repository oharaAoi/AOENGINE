#include "BaseParticles.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"

void BaseParticles::Init(const std::string& name) {
	particleName_ = name;
	SetName(name);
	
	/*shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();*/

	emitter_.FromJson(JsonItems::GetData(kGroupName, particleName_));
	if (emitter_.useMesh == "") {
		shape_ = MeshManager::GetInstance()->GetMesh("plane");
	} else {
		shape_ = MeshManager::GetInstance()->GetMesh(emitter_.useMesh);
	}
	
	shape_->SetUseMaterial(emitter_.useTexture);

	isAddBlend_ = emitter_.isParticleAddBlend;
	emitAccumulator_ = 0.0f;
	isStop_ = true;

	changeMesh_ = false;
}

void BaseParticles::Update() {

	// ---------------------------
	// Emitterの更新
	// ---------------------------
	EmitUpdate();
}

void BaseParticles::Emit(const Vector3& pos) {
	if (particleArray_->size() >= kMaxParticles) { return; }

	auto& newParticle = particleArray_->emplace_back();

	newParticle.scale = RandomVector3(emitter_.minScale, emitter_.maxScale);
	newParticle.firstScale = newParticle.scale;
	newParticle.rotate = Quaternion();

	// particleの出現位置を設定
	if (emitter_.emitOrigin == 0) {
		newParticle.translate = pos;
	} else if (emitter_.emitOrigin == 1) {
		float rangeX = RandomFloat(-emitter_.radius, emitter_.radius);
		float rangeY = RandomFloat(-emitter_.radius, emitter_.radius);
		float rangeZ = RandomFloat(-emitter_.radius, emitter_.radius);
		newParticle.translate = Vector3(rangeX, rangeY, rangeZ) + pos;
	} else if (emitter_.emitOrigin == 2 || emitter_.emitOrigin == 3) {
		float u = RandomFloat(0, 1);
		float v = RandomFloat(0, 1);

		float theta = u * 2.0f * kPI;
		float phi = std::acos(2.0f * v - 1.0f);

		Vector3 dire;
		dire.x = std::sin(phi) * std::cos(theta);
		dire.x = std::sin(phi) * std::sin(theta);
		dire.x = std::cos(phi);

		newParticle.translate = pos + (dire * emitter_.radius);
	 }

	newParticle.translate = pos;
	newParticle.color = emitter_.color;
	if (emitter_.shape == 0) {
		// 親の回転成分を取り出す
		Quaternion rotate;
		if (parentWorldMat_ != nullptr) {
			rotate = parentWorldMat_->GetRotate();
		} else {
			rotate = Quaternion();
		}
		newParticle.velocity = (rotate * (RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT).Normalize())) * emitter_.speed;
	} else if (emitter_.shape == 1) {
		// 親の回転成分を取り出す
		Quaternion rotate;
		if (parentWorldMat_ != nullptr) {
			rotate = parentWorldMat_->GetRotate();
		} else {
			rotate = Quaternion();
		}
		Vector3 randVector3 = RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT).Normalize() * 0.1f;
		newParticle.velocity = (rotate * (emitter_.direction.Normalize() + randVector3).Normalize()) * emitter_.speed;
	}

	// particleの方向を設定
	if (emitter_.emitType == 0) {
		newParticle.velocity = CVector3::UP * emitter_.speed;
	} else if (emitter_.emitType == 1) {
		Vector3 dire = RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT);
		newParticle.velocity = dire * emitter_.speed;
	} else if (emitter_.emitType == 2) {
		newParticle.velocity = (newParticle.translate - pos).Normalize() * emitter_.speed;
	}

	// billbordに合わせてz軸を進行方向に向ける
	if (emitter_.isDirectionRotate) {
		Vector3 forward = newParticle.velocity.Normalize();

		// 上方向（カメラ視点に合わせるなら bill.MakeMatrix() などから取得も可）
		Vector3 up = CVector3::UP;

		// forwardとupが平行だと問題なのでチェック
		if (fabsf(Dot(forward, up)) > 0.99f) {
			up = CVector3::RIGHT;
		}

		// オルソン直交基底を構築（右・上・前）
		Vector3 right = Cross(up, forward).Normalize();
		Vector3 adjustedUp = Cross(forward, right).Normalize();

		// 回転行列を作成（Z軸 = forward）
		Matrix4x4 rotMat;
		rotMat.m[0][0] = right.x;   rotMat.m[0][1] = right.y;   rotMat.m[0][2] = right.z;   rotMat.m[0][3] = 0;
		rotMat.m[1][0] = adjustedUp.x; rotMat.m[1][1] = adjustedUp.y; rotMat.m[1][2] = adjustedUp.z; rotMat.m[1][3] = 0;
		rotMat.m[2][0] = forward.x; rotMat.m[2][1] = forward.y; rotMat.m[2][2] = forward.z; rotMat.m[2][3] = 0;
		rotMat.m[3][0] = 0;         rotMat.m[3][1] = 0;         rotMat.m[3][2] = 0;         rotMat.m[3][3] = 1;

		// 行列からクォータニオンへ変換
		newParticle.rotate = Quaternion::FromMatrix(rotMat);
	}

	// EmitterからParticleのパラメータを取得する
	newParticle.lifeTime = emitter_.lifeTime;
	newParticle.firstLifeTime = emitter_.lifeTime;
	newParticle.currentTime = 0.0f;
	newParticle.damping = emitter_.dampig;
	newParticle.gravity = emitter_.gravity;

	newParticle.isLifeOfAlpha = emitter_.isLifeOfAlpha;
	newParticle.isLifeOfScale = emitter_.isLifeOfScale;
	newParticle.isAddBlend = emitter_.isParticleAddBlend;

	newParticle.isScaleUpScale = emitter_.isScaleUp;
	newParticle.upScale = emitter_.scaleUpScale;

	newParticle.stretchBillboard = emitter_.stretchBillboard;
	newParticle.stretchScaleFactor = emitter_.stretchScale;
	newParticle.isBillBord = emitter_.isBillBord;
}

void BaseParticles::EmitUpdate() {
	if (isStop_) { return; }

	// 一度だけ打つフラグがtrueだったら
	if (!emitter_.isLoop) {
		for (uint32_t count = 0; count < emitter_.rateOverTimeCout; ++count) {
			if (parentWorldMat_ != nullptr) {
				Emit(emitter_.translate + parentWorldMat_->GetPosition());
			} else {
				Emit(emitter_.translate);
			}
		}
		isStop_ = true;
	}

	// 射出のflagがtrueだったら
	emitAccumulator_ += emitter_.rateOverTimeCout * GameTimer::DeltaTime();
	// 発射すべき個数を計算する
	int emitCout = static_cast<int>(emitAccumulator_);
	for (int count = 0; count < emitCout; ++count) {
		if (parentWorldMat_ != nullptr) {
			Emit(emitter_.translate + parentWorldMat_->GetPosition());
		} else {
			Emit(emitter_.translate);
		}
	}
	emitAccumulator_ -= emitCout;

	// 継続時間を進める
	currentTimer_ += GameTimer::DeltaTime();
	if (currentTimer_ > emitter_.duration) {
		if (!emitter_.isLoop) {
			isStop_ = true;
		}
	}
	
}
void BaseParticles::Reset() {
	emitAccumulator_ = 0.0f;
	currentTimer_ = 0.0f;
	isStop_ = false;
}

void BaseParticles::Debug_Gui() {
	if (ImGui::Button("Reset")) {
		Reset();
	}

	ImGui::Checkbox("IsStop", &isStop_);
	emitter_.Attribute_Gui();

	ImGui::Separator();

	ImGui::Text("Particle Parameters");
	shareMaterial_->Debug_Gui();
	
	emitter_.useTexture = shareMaterial_->GetAlbedoTexture();
	shareMaterial_->SetAlbedoTexture(emitter_.useTexture);

	meshName_ = MeshManager::GetInstance()->SelectMeshName();
	if (ImGui::Button("ChangeMesh")) {
		changeMesh_ = true;
	}

	if (ImGui::Button("Save")) {
		JsonItems::Save("CPU", emitter_.ToJson(particleName_), "Effect");
	}
	if (ImGui::Button("Apply")) {
		emitter_.FromJson(JsonItems::GetData("CPU", particleName_));
	}
}

void BaseParticles::ChangeMesh() {
	emitter_.useMesh = meshName_;
	shape_ = MeshManager::GetInstance()->GetMesh(meshName_);
	changeMesh_ = false;
}

void BaseParticles::SetParent(const Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}
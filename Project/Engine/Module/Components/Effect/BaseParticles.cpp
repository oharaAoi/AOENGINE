#include "BaseParticles.h"
#include "Engine/Render.h"
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Render.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::Init(const std::string& name) {
	particleName_ = name;
	SetName(name);

	// meshの設定
	emitter_.SetGroupName("CPU");
	emitter_.SetName(particleName_);
	emitter_.SetRootField(JsonItems::GetDirectoryPath() + "Effect/");
	emitter_.Load();
	if (emitter_.useMesh == "") {
		shape_ = AOENGINE::MeshManager::GetInstance()->GetMesh("plane");
	} else {
		shape_ = AOENGINE::MeshManager::GetInstance()->GetMesh(emitter_.useMesh);
	}

	// materialの設定
	shape_->SetUseMaterial(emitter_.useTexture);

	isAddBlend_ = emitter_.isParticleAddBlend;
	emitAccumulator_ = 0.0f;
	isStop_ = true;

	changeMesh_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::Update() {
	EmitUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::DrawShape() {
	Math::Matrix4x4 mat{};
	if (!emitter_.isDraw2d) {
		mat = AOENGINE::Render::GetViewProjectionMat();
	} else {
		mat = AOENGINE::Render::GetProjection2D();
	}

	// 形状の描画
	if (emitter_.shape == (int)CpuEmitterShape::Box) {
		Math::OBB obb = {
			.center = emitter_.translate,
			.size = emitter_.size
		};
		obb.MakeOBBAxis(Math::Quaternion::EulerToQuaternion(emitter_.rotate));
		DrawOBB(obb, mat, Colors::Linear::green);
	} else if (emitter_.shape == (int)CpuEmitterShape::Shpere) {
		DrawSphere(emitter_.translate, emitter_.radius, mat, Colors::Linear::green);
	} else if (emitter_.shape == (int)CpuEmitterShape::Cone) {
		Math::Quaternion rotate = Math::Quaternion::EulerToQuaternion(emitter_.rotate);
		DrawCone(emitter_.translate, rotate, emitter_.radius, emitter_.angle, emitter_.height, mat);
	}
}

void AOENGINE::BaseParticles::Emit(const Math::Vector3& pos) {
	if (particleArray_->size() >= kMaxParticles) { return; }

	auto& newParticle = particleArray_->emplace_back();

	// scaleの決定
	if (emitter_.separateByAxisScale) {
		newParticle.scale = Random::RandomVector3(emitter_.minScale, emitter_.maxScale);
	} else {
		float scaler = Random::RandomFloat(emitter_.minScale.x, emitter_.maxScale.x);
		newParticle.scale = Math::Vector3(scaler, scaler, scaler);
	}

	if (emitter_.isRandomRotate) {
		newParticle.rotateZ = Random::RandomFloat(emitter_.minAngle, emitter_.maxAngle);
	}

	newParticle.firstScale = newParticle.scale;
	newParticle.rotate = Math::Quaternion::AngleAxis(newParticle.rotateZ * kToRadian, CVector3::FORWARD);

	// particleの出現位置を設定
	if (emitter_.emitOrigin == (int)CpuEmitOrigin::Center) {
		newParticle.translate = pos;

	} else if (emitter_.emitOrigin == (int)CpuEmitOrigin::Range) {
		if (emitter_.shape == (int)CpuEmitterShape::Box) {
			float rangeX = Random::RandomFloat(-emitter_.size.x, emitter_.size.x);
			float rangeY = Random::RandomFloat(-emitter_.size.y, emitter_.size.y);
			float rangeZ = Random::RandomFloat(-emitter_.size.z, emitter_.size.z);
			newParticle.translate = Math::Vector3(rangeX, rangeY, rangeZ) + pos;
		} else if (emitter_.shape == (int)CpuEmitterShape::Shpere) {
			float rangeX = Random::RandomFloat(-emitter_.radius, emitter_.radius);
			float rangeY = Random::RandomFloat(-emitter_.radius, emitter_.radius);
			float rangeZ = Random::RandomFloat(-emitter_.radius, emitter_.radius);
			newParticle.translate = Math::Vector3(rangeX, rangeY, rangeZ) + pos;
		} else if (emitter_.shape == (int)CpuEmitterShape::Cone) {
			float rangeX = Random::RandomFloat(-emitter_.radius, emitter_.radius);
			float rangeZ = Random::RandomFloat(-emitter_.radius, emitter_.radius);
			newParticle.translate = Math::Vector3(rangeX, 0.f, rangeZ) + pos;
		}
	}

	// 色の決定
	if (emitter_.isRandomColor) {
		float t = Random::RandomFloat(0.f, 1.f);
		newParticle.color.r = Lerp(emitter_.randColor1.r, emitter_.randColor2.r, t);
		t = Random::RandomFloat(0.f, 1.f);
		newParticle.color.g = Lerp(emitter_.randColor1.g, emitter_.randColor2.g, t);
		t = Random::RandomFloat(0.f, 1.f);
		newParticle.color.b = Lerp(emitter_.randColor1.b, emitter_.randColor2.b, t);
	} else {
		newParticle.color = emitter_.color;
	}

	// particleの方向を設定
	if (emitter_.emitDirection == (int)CpuEmitDirection::Up) {
		newParticle.velocity = CVector3::UP * emitter_.speed;
	} else if (emitter_.emitDirection == (int)CpuEmitDirection::Random) {
		Math::Vector3 dire = Random::RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT);
		newParticle.velocity = dire * emitter_.speed;

	} else if (emitter_.emitDirection == (int)CpuEmitDirection::Outside) {
		newParticle.velocity = (newParticle.translate - pos).Normalize() * emitter_.speed;

	} else if (emitter_.emitDirection == (int)CpuEmitDirection::CenterFor) {
		newParticle.velocity = (pos - newParticle.translate).Normalize() * emitter_.speed;
	}

	// Coneの場合はConeの形状で射出させる
	if (emitter_.shape == (int)CpuEmitterShape::Cone) {
		float angle = emitter_.angle * kToRadian;
		float u = Random::RandomFloat(0, 1);
		float v = Random::RandomFloat(0, 1);

		float phi = 2.0f * kPI * u;
		float cosTheta = Lerp(cos(angle), 1.0f, v);
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		Math::Vector3 localDir(
			sinTheta * cos(phi),
			sinTheta * sin(phi),
			cosTheta
		);
		Math::Vector3 up = CVector3::UP;
		Math::Vector3 right = CVector3::RIGHT;
		Math::Vector3 forward = CVector3::FORWARD;
		newParticle.velocity = right * localDir.x + forward * localDir.y + CVector3::UP * localDir.z;
	}

	// Objectの回転に進行方向をあわせる
	Math::Vector3 dire = newParticle.velocity.Normalize();
	Math::Vector3 worldDire = Math::Quaternion::EulerToQuaternion(emitter_.rotate) * dire;
	newParticle.velocity = worldDire * emitter_.speed;

	// billbordに合わせてz軸を進行方向に向ける
	if (emitter_.isDirectionRotate) {
		Math::Vector3 forward = newParticle.velocity.Normalize();

		// 上方向（カメラ視点に合わせるなら bill.MakeMatrix() などから取得も可）
		Math::Vector3 up = CVector3::UP;

		// forwardとupが平行だと問題なのでチェック
		if (fabsf(Dot(forward, up)) > 0.99f) {
			up = CVector3::RIGHT;
		}

		// オルソン直交基底を構築（右・上・前）
		Math::Vector3 right = Cross(up, forward).Normalize();
		Math::Vector3 adjustedUp = Cross(forward, right).Normalize();

		// 回転行列を作成（Z軸 = forward）
		Math::Matrix4x4 rotMat;
		rotMat.m[0][0] = right.x;   rotMat.m[0][1] = right.y;   rotMat.m[0][2] = right.z;   rotMat.m[0][3] = 0;
		rotMat.m[1][0] = adjustedUp.x; rotMat.m[1][1] = adjustedUp.y; rotMat.m[1][2] = adjustedUp.z; rotMat.m[1][3] = 0;
		rotMat.m[2][0] = forward.x; rotMat.m[2][1] = forward.y; rotMat.m[2][2] = forward.z; rotMat.m[2][3] = 0;
		rotMat.m[3][0] = 0;         rotMat.m[3][1] = 0;         rotMat.m[3][2] = 0;         rotMat.m[3][3] = 1;

		// 行列からクォータニオンへ変換
		newParticle.rotate = Math::Quaternion::FromMatrix(rotMat);
	}

	// EmitterからParticleのパラメータを取得する
	newParticle.isLerpDiscardValue = emitter_.isLerpDiscardValue;
	newParticle.discardValue = emitter_.discardValue;
	newParticle.startDiscard = emitter_.startDiscard;
	newParticle.endDiscard = emitter_.endDiscard;

	newParticle.lifeTime = emitter_.lifeTime;
	newParticle.currentTime = 0.0f;
	newParticle.damping = emitter_.dampig;
	newParticle.gravity = emitter_.gravity;

	newParticle.isLifeOfAlpha = emitter_.isLifeOfAlpha;
	newParticle.isLifeOfScale = emitter_.isLifeOfScale;
	newParticle.lifeOfMinScale = emitter_.lifeOfMinScale;
	newParticle.lifeOfMaxScale = emitter_.lifeOfMaxScale;
	newParticle.isAddBlend = emitter_.isParticleAddBlend;

	newParticle.isFadeInOut = emitter_.isFadeInOut;
	newParticle.fadeInTime = emitter_.fadeInTime;
	newParticle.fadeOutTime = emitter_.fadeOutTime;
	newParticle.initAlpha_ = emitter_.color.a;

	newParticle.isScaleUpScale = emitter_.isScaleUp;
	newParticle.upScale = emitter_.scaleUpScale;

	newParticle.isStretch = emitter_.isStretch;
	newParticle.isBillBord = emitter_.isBillBord;
	newParticle.isDraw2d = emitter_.isDraw2d;

	newParticle.isTextureAnimation = emitter_.isTextureSheetAnimation;
	newParticle.tileSize = emitter_.tiles;

	newParticle.isColorAnimation = emitter_.isColorAnimation;
	if (emitter_.isColorAnimation) {
		newParticle.preColor = emitter_.preColor;
		newParticle.postColor = emitter_.postColor;
	}

	if (emitter_.emitDirection == (int)CpuEmitDirection::CenterFor) {
		newParticle.isCenterFor = true;
	} else {
		newParticle.isCenterFor = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 射出更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::EmitUpdate() {

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

	float deltaTime = AOENGINE::GameTimer::DeltaTime();
	// 射出のflagがtrueだったら
	emitAccumulator_ = emitter_.rateOverTimeCout * deltaTime;
	// 発射すべき個数を計算する
	int emitCout = static_cast<int>(emitAccumulator_);
	for (int count = 0; count < emitCout; ++count) {
		float t = 0;
		if (count > 1) {
			t = (count) / float(emitCout - 1);
		}
		if (parentWorldMat_ != nullptr) {
			Math::Vector3 pos = Math::Vector3::Lerp(preWorldPos_, emitter_.translate + parentWorldMat_->GetPosition(), t);
			Emit(pos);
		} else {
			Math::Vector3 pos = Math::Vector3::Lerp(emitter_.preTranslate, emitter_.translate, t);
			Emit(pos);
		}
	}
	
	// 継続時間を進める
	currentTimer_ += deltaTime;
	if (currentTimer_ > emitter_.duration) {
		if (!emitter_.isLoop) {
			isStop_ = true;
		}
	}

	emitter_.preTranslate = emitter_.translate;
	if (parentWorldMat_ != nullptr) {
		preWorldPos_ = emitter_.translate + parentWorldMat_->GetPosition();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ リセット処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::Reset() {
	emitAccumulator_ = 0.0f;
	currentTimer_ = 0.0f;
	isStop_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::Debug_Gui() {
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

	meshName_ = AOENGINE::MeshManager::GetInstance()->SelectMeshName();
	if (ImGui::Button("ChangeMesh")) {
		changeMesh_ = true;
	}

	emitter_.SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ meshの変更
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::ChangeMesh() {
	emitter_.useMesh = meshName_;
	shape_ = AOENGINE::MeshManager::GetInstance()->GetMesh(meshName_);
	changeMesh_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 親の設定
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::SetParent(const Math::Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}
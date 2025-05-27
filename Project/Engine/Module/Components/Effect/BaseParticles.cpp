#include "BaseParticles.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"

void BaseParticles::Init(const std::string& name) {
	name_ = name;
	
	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();
	
	emitter_.FromJson(JsonItems::GetData(kGroupName, name_));
	shape_->GetMaterial()->SetUseTexture(emitter_.useTexture);

	isAddBlend_ = emitter_.isParticleAddBlend;
	emitAccumulator_ = 0.0f;

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, name_);
#endif // _DEBUG
}

void BaseParticles::Update() {

	// ---------------------------
	// Emitterの更新
	// ---------------------------
	EmitUpdate();

	// ---------------------------
	// particleの更新
	// ---------------------------
	/*size_t particleNum = particleArray_.size();
	data_.resize(particleNum);
	for (uint32_t oi = 0; oi < particleNum; ++oi) {
		data_[oi].color.w = 0.0f;
	}*/

	//size_t index = 0;
	//for (auto it = particleArray_.begin(); it != particleArray_.end();) {
	//	auto& pr = *it;
	//	// ---------------------------
	//	// 生存時間の更新
	//	// ---------------------------
	//	pr.lifeTime -= GameTimer::DeltaTime();
	//	if (pr.lifeTime <= 0.0f) {
	//		it = particleArray_.erase(it); // 削除して次の要素にスキップ
	//		continue;
	//	}

	//	// ---------------------------
	//	// Parameterの更新
	//	// ---------------------------
	//	// 速度を更新
	//	pr.velocity *= std::powf((1.0f - pr.damping), GameTimer::DeltaTime());

	//	// 重力を適応
	//	pr.velocity.y += pr.gravity * GameTimer::DeltaTime();

	//	// 座標を適応
	//	pr.translate += pr.velocity * GameTimer::DeltaTime();

	//	// ---------------------------
	//	// 状態の更新
	//	// ---------------------------
	//	float t = pr.lifeTime / pr.firstLifeTime;
	//	t = 1.0f - t;
	//	if (pr.isLifeOfAlpha) {
	//		pr.color.w = Lerp(1.0f, 0.0f, t);
	//	}

	//	if (pr.isLifeOfScale) {
	//		pr.scale = Vector3::Lerp(pr.firstScale, CVector3::ZERO, t);
	//	}

	//	if (pr.isScaleUpScale) {
	//		float scaleT = pr.lifeTime / pr.firstLifeTime;
	//		scaleT = 1.0f - scaleT;
	//		pr.scale = Vector3::Lerp(CVector3::ZERO, pr.upScale, scaleT);
	//	}

	//	Matrix4x4 scaleMatrix = pr.scale.MakeScaleMat();
	//	Matrix4x4 billMatrix = bill.MakeMatrix(); // ← ビルボード行列（カメラからの視線で作る）
	//	Matrix4x4 zRot = pr.rotate.MakeMatrix();
	//	Matrix4x4 rotateMatrix = Multiply(zRot, Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), billMatrix));
	//	Matrix4x4 translateMatrix = pr.translate.MakeTranslateMat();

	//	Matrix4x4 localWorld = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

	//	//if (parentWorldMat_ != nullptr) {
	//	//	// 親の位置情報だけを抽出（回転・スケールを無視）
	//	//	Vector3 parentPos = parentWorldMat_->GetPosition();
	//	//	Matrix4x4 parentTranslate = parentPos.MakeTranslateMat();

	//	//	data_[index].worldMat = localWorld * parentTranslate;
	//	//} else {
	//	//	data_[index].worldMat = localWorld;
	//	//}

	//	data_[index].worldMat = localWorld;
	//	data_[index].color = pr.color;

	//	// ---------------------------
	//	// NextFrameのための更新
	//	// ---------------------------
	//	++index;
	//	++it;
	//}
}

void BaseParticles::Emit(const Vector3& pos) {
	if (particleArray_->size() >= kMaxParticles) { return; }

	auto& newParticle = particleArray_->emplace_back();

	newParticle.scale = RandomVector3(emitter_.minScale, emitter_.maxScale);
	newParticle.firstScale = newParticle.scale;
	newParticle.rotate = Quaternion::AngleAxis(RandomFloat(emitter_.angleMin, emitter_.angleMax), CVector3::FORWARD);
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

	newParticle.isScaleUpScale = emitter_.isScaleUp;
	newParticle.upScale = emitter_.scaleUpScale;
}

void BaseParticles::EmitUpdate() {
	// 一度だけ打つフラグがtrueだったら
	if (emitter_.isOneShot) {
		for (uint32_t count = 0; count < emitter_.rateOverTimeCout; ++count) {
			if (parentWorldMat_ != nullptr) {
				Emit(emitter_.translate + parentWorldMat_->GetPosition());
			} else {
				Emit(emitter_.translate);
			}
		}
		emitter_.isOneShot = false;
	}

	// 射出のflagがtrueだったら
	if (!emitter_.emit) { return; }
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
}

#ifdef _DEBUG
void BaseParticles::Debug_Gui() {
	emitter_.Attribute_Gui();

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Text("Particle Parameters");
		shape_->GetMaterial()->ImGuiDraw();
	}
	emitter_.useTexture = shape_->GetMaterial()->GetUseTexture();

	if (ImGui::Button("Save")) {
		JsonItems::Save(kGroupName, emitter_.ToJson(name_));
	}
	if (ImGui::Button("Apply")) {
		emitter_.FromJson(JsonItems::GetData(kGroupName, name_));
	}
}
#endif

void BaseParticles::SetParent(const Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}
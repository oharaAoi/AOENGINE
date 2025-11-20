#include "CpuParticles.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CpuParticles::Init(const std::string& name, bool isAddBlend) {
	name_ = name;
	isAddBlend_ = isAddBlend;

	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();

	emitter_.FromJson(JsonItems::GetData(kGroupName, name_));
	shape_->GetMaterial()->SetAlbedoTexture(emitter_.useTexture);

	emitAccumulator_ = 0.0f;

	isStop_ = false;

#ifdef _DEBUG
	EditorWindows::AddObjectWindow(this, name_);
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CpuParticles::Update(const Quaternion& bill) {

	// ---------------------------
	// Emitterの更新
	// ---------------------------
	EmitUpdate();

	// ---------------------------
	// particleの更新
	// ---------------------------
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
		// 速度を更新
		pr.velocity *= std::powf((1.0f - pr.damping), GameTimer::DeltaTime());

		// 重力を適応
		pr.velocity.y += pr.gravity * GameTimer::DeltaTime();

		// 座標を適応
		pr.translate += pr.velocity * GameTimer::DeltaTime();

		Matrix4x4 scaleMatrix = pr.scale.MakeScaleMat();
		Matrix4x4 billMatrix = bill.MakeMatrix(); // ← ビルボード行列（カメラからの視線で作る）
		Matrix4x4 zRot = pr.rotate.MakeMatrix();
		Matrix4x4 rotateMatrix = Multiply(zRot, Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), billMatrix));
		Matrix4x4 translateMatrix = pr.translate.MakeTranslateMat();

		Matrix4x4 localWorld = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

		if (parentWorldMat_ != nullptr) {
			// 親の位置情報だけを抽出（回転・スケールを無視）
			Vector3 parentPos = parentWorldMat_->GetPosition();
			Matrix4x4 parentTranslate = parentPos.MakeTranslateMat();

			data[index].worldMat = localWorld * parentTranslate;
		} else {
			data[index].worldMat = localWorld;
		}

		data[index].color = pr.color;

		// ---------------------------
		// NextFrameのための更新
		// ---------------------------
		++index;
		++it;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 射出
///////////////////////////////////////////////////////////////////////////////////////////////

void CpuParticles::Emit(const Vector3& pos) {
	if (particleArray_.size() >= kMaxParticles) { return; }

	auto& newParticle = particleArray_.emplace_back();

	// scaleの決定
	if (emitter_.separateByAxisScale) {
		newParticle.scale = RandomVector3(emitter_.minScale, emitter_.maxScale);
	} else {
		float scaler = RandomFloat(emitter_.minScale.x, emitter_.maxScale.x);
		newParticle.scale = Vector3(scaler, scaler, scaler);
	}

	newParticle.firstScale = newParticle.scale;
	newParticle.rotate = Quaternion();

	// particleの出現位置を設定
	if (emitter_.emitOrigin == (int)CpuEmitOrigin::Center) {
		newParticle.translate = pos;

	} else if (emitter_.emitOrigin == (int)CpuEmitOrigin::Range) {
		if (emitter_.shape == (int)CpuEmitterShape::Box) {
			float rangeX = RandomFloat(-emitter_.size.x, emitter_.size.x);
			float rangeY = RandomFloat(-emitter_.size.y, emitter_.size.y);
			float rangeZ = RandomFloat(-emitter_.size.z, emitter_.size.z);
			newParticle.translate = Vector3(rangeX, rangeY, rangeZ) + pos;
		} else if (emitter_.shape == (int)CpuEmitterShape::Shere) {
			float rangeX = RandomFloat(-emitter_.radius, emitter_.radius);
			float rangeY = RandomFloat(-emitter_.radius, emitter_.radius);
			float rangeZ = RandomFloat(-emitter_.radius, emitter_.radius);
			newParticle.translate = Vector3(rangeX, rangeY, rangeZ) + pos;
		} else {
			newParticle.translate = pos;
		}
	}

	// 色の決定
	if (emitter_.isRandomColor) {
		float t = RandomFloat(0.f, 1.f);
		newParticle.color.r = Lerp(emitter_.randColor1.r, emitter_.randColor2.r, t);
		t = RandomFloat(0.f, 1.f);
		newParticle.color.g = Lerp(emitter_.randColor1.g, emitter_.randColor2.g, t);
		t = RandomFloat(0.f, 1.f);
		newParticle.color.b = Lerp(emitter_.randColor1.b, emitter_.randColor2.b, t);
	} else {
		newParticle.color = emitter_.color;
	}

	// particleの方向を設定
	if (emitter_.emitDirection == (int)CpuEmitDirection::Up) {
		newParticle.velocity = CVector3::UP * emitter_.speed;
	} else if (emitter_.emitDirection == (int)CpuEmitDirection::Random) {
		Vector3 dire = RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT);
		newParticle.velocity = dire * emitter_.speed;

	} else if (emitter_.emitDirection == (int)CpuEmitDirection::Outside) {
		newParticle.velocity = (newParticle.translate - pos).Normalize() * emitter_.speed;

	} else if (emitter_.emitDirection == (int)CpuEmitDirection::CenterFor) {
		newParticle.velocity = (pos - newParticle.translate).Normalize() * emitter_.speed;
	}

	// Coneの場合はConeの形状で射出させる
	if (emitter_.shape == (int)CpuEmitterShape::Cone) {
		float angle = emitter_.angle * kToRadian;
		float u = RandomFloat(0, 1);
		float v = RandomFloat(0, 1);

		float phi = 2.0f * kPI * u;
		float cosTheta = Lerp(cos(angle), 1.0f, v);
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		Vector3 localDir(
			sinTheta * cos(phi),
			sinTheta * sin(phi),
			cosTheta
		);
		Vector3 up = CVector3::UP;
		Vector3 right = CVector3::RIGHT;
		Vector3 forward = CVector3::FORWARD;
		newParticle.velocity = right * localDir.x + forward * localDir.y + CVector3::UP * localDir.z;
	}

	// Objectの回転に進行方向をあわせる
	Vector3 dire = newParticle.velocity.Normalize();
	Vector3 worldDire = Quaternion::EulerToQuaternion(emitter_.rotate) * dire;
	newParticle.velocity = worldDire * emitter_.speed;

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
	newParticle.currentTime = 0.0f;
	newParticle.damping = emitter_.dampig;
	newParticle.gravity = emitter_.gravity;

	newParticle.isLifeOfAlpha = emitter_.isLifeOfAlpha;
	newParticle.isLifeOfScale = emitter_.isLifeOfScale;
	newParticle.isAddBlend = emitter_.isParticleAddBlend;

	newParticle.isFadeInOut = emitter_.isFadeInOut;
	newParticle.fadeInTime = emitter_.fadeInTime;
	newParticle.fadeOutTime = emitter_.fadeOutTime;
	newParticle.initAlpha_ = emitter_.color.a;

	newParticle.isScaleUpScale = emitter_.isScaleUp;
	newParticle.upScale = emitter_.scaleUpScale;

	newParticle.isBillBord = emitter_.isBillBord;
	newParticle.isDraw2d = emitter_.isDraw2d;
	if (emitter_.emitDirection == (int)CpuEmitDirection::CenterFor) {
		newParticle.isCenterFor = true;
	} else {
		newParticle.isCenterFor = false;
	}
	newParticle.isDraw2d = emitter_.isDraw2d;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 射出の更新
///////////////////////////////////////////////////////////////////////////////////////////////

void CpuParticles::EmitUpdate() {
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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CpuParticles::Debug_Gui() {
	emitter_.Attribute_Gui();

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Text("Particle Parameters");
		shape_->GetMaterial()->Debug_Gui();
	}
	emitter_.useTexture = shape_->GetMaterial()->GetAlbedoTexture();

	if (ImGui::Button("Save")) {
		JsonItems::Save(kGroupName, emitter_.ToJson(name_));
	}
	if (ImGui::Button("Apply")) {
		emitter_.FromJson(JsonItems::GetData(kGroupName, name_));
	}
}
#endif

void CpuParticles::SetParent(const Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}
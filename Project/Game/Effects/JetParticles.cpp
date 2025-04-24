#include "JetParticles.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"

void JetParticles::Init(const std::string& name) {
	name_ = name;

	shape_ = std::make_unique<GeometryObject>();
	shape_->Set<PlaneGeometry>();
	ParticleManager::GetInstance()->AddParticle(name_, shape_->GetMesh(), shape_->GetMaterial());

	emitter_.FromJson(JsonItems::GetData(kGroupName, name_));
	shape_->GetMaterial()->SetUseTexture(emitter_.useTexture);

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, name_);
#endif // _DEBUG
}

void JetParticles::Update(const Quaternion& bill) {

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

		// ---------------------------
		// Rendererに送る情報を更新
		// ---------------------------
		Matrix4x4 scaleMatrix = pr.scale.MakeScaleMat();
		Matrix4x4 billMatrix = bill.MakeMatrix(); // ← ビルボード行列（カメラからの視線で作る）
		Matrix4x4 rotateMatrix = Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), billMatrix);
		Matrix4x4 translateMatrix = pr.translate.MakeTranslateMat();

		Matrix4x4 localWorld = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

		if (parentWorldMat_ != nullptr) {
			// 親の位置情報だけを抽出（回転・スケールを無視）
			Vector3 parentPos = parentWorldMat_->GetPos();
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
	ParticleManager::GetInstance()->Update(name_, data);
}

void JetParticles::Emit(const Vector3& pos) {
	if (particleArray_.size() >= 100) { return; }
	for (uint32_t oi = 0; oi < emitter_.count; ++oi) {
		auto& newParticle = particleArray_.emplace_back();

		newParticle.scale = RandomVector3(emitter_.minScale, emitter_.maxScale);
		newParticle.rotate = Quaternion();
		newParticle.translate = pos;
		newParticle.color = emitter_.color;
		if (emitter_.shape == 0) {
			newParticle.velocity = RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT).Normalize() * emitter_.speed;
		} else if (emitter_.shape == 1) {
			Vector3 randVector3 = RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT).Normalize() * 0.1f;
			newParticle.velocity = (emitter_.direction.Normalize() + randVector3).Normalize() * emitter_.speed;
		}
		newParticle.lifeTime = emitter_.lifeTime;
		newParticle.currentTime = 0.0f;
		newParticle.damping = emitter_.dampig;
		newParticle.gravity = emitter_.gravity;
	}
}

void JetParticles::EmitUpdate() {
	if (!emitter_.emit) { return; }
	emitter_.frequencyTime += GameTimer::DeltaTime();

	if (emitter_.frequencyTime > emitter_.frequency) {
		Emit(emitter_.translate);
		emitter_.frequencyTime = 0.0f;
	}
}

#ifdef _DEBUG
void JetParticles::Debug_Gui() {
	emitter_.Attribute_Gui();

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Text("Particle Parameters");
		shape_->GetMaterial()->ImGuiDraw();
	}
	emitter_.useTexture = shape_->GetMaterial()->GetUseTexture();

	if (ImGui::Button("emit")) {
		Emit(emitter_.translate);
	}

	if (ImGui::Button("Save")) {
		JsonItems::Save(kGroupName, emitter_.ToJson(name_));
	}
	if (ImGui::Button("Apply")) {
		emitter_.FromJson(JsonItems::GetData(kGroupName, name_));
	}
}
#endif

void JetParticles::SetParent(const Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}
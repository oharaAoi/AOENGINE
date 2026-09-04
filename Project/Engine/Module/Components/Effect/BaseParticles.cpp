#include "BaseParticles.h"
#include "Engine/Render/Render.h"
#include <algorithm>
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Asset/AssetHandle.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Render/Render.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

BaseParticles::~BaseParticles() {
	if (registeredForGameView_) {
		ParticleManager::GetInstance()->UnregisterExternalParticle(this);
	}
}

void AOENGINE::BaseParticles::Init(const std::string& name, bool registerForGameView) {
	particleName_ = name;
	SetName(name);

	// meshの設定
	emitter_.SetGroupName("CPU");
	emitter_.SetName(particleName_);
	emitter_.SetRootField("Effect/");
	emitter_.Load();
	if (emitter_.useMesh == "") {
		shape_ = AOENGINE::MeshManager::GetInstance()->GetMesh("plane");
	} else {
		shape_ = AOENGINE::MeshManager::GetInstance()->GetMesh(emitter_.useMesh);
	}

	// materialの設定
	shape_->SetUseMaterial(emitter_.useTexture);

	blendModeType_ = emitter_.blendModeType;
	emitAccumulator_ = 0.0f;
	distanceAccumulator_ = 0.0f;
	currentTimer_ = 0.0f;
	isStop_ = false;
	changeMesh_ = false;

	worldTransform_ = std::make_unique<WorldTransform>();
	worldTransform_->Init();
	worldTransform_->SetRotate(Math::Quaternion::EulerToQuaternion(emitter_.rotate));
	worldTransform_->SetTranslate(emitter_.translate);

	if (registerForGameView) {
		registeredForGameView_ = ParticleManager::GetInstance()->RegisterExternalParticle(this);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::Update() {
	const uint32_t capacity = (std::max)(emitter_.maxParticles, 1u);
	while (particleArray_->size() > capacity) {
		particleArray_->pop_front();
	}

	// 回転の更新
	Math::Quaternion rotate = Math::Quaternion::EulerToQuaternion(emitter_.rotate);
	if (parentTransform_ != nullptr) {
		Math::QuaternionSRT srt = parentTransform_->GetWorldSRT();
		rotate = srt.rotate * rotate;
	}
	worldTransform_->SetRotate(rotate);

	// 座標の更新
	Math::Vector3 localPos = emitter_.translate;
	Math::Vector3 worldPos = localPos;

	if (parentTransform_ != nullptr) {
		worldPos = TransformCoord(localPos, parentTransform_->GetWorldMatrix());
	}
	if (parentMatrix_ != nullptr) {
		worldPos = TransformCoord(localPos, *parentMatrix_);
	}
	worldTransform_->SetTranslate(worldPos);

	worldTransform_->Update();
	if (!hasPreWorldPos_) {
		preWorldPos_ = worldTransform_->GetWorldPos();
		hasPreWorldPos_ = true;
	}
	// 射出の更新を行う
	EmitUpdate();
	preWorldPos_ = worldTransform_->GetWorldPos();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::DrawShape() {
	Math::Matrix4x4 mat{};
	Math::Quaternion worldRotate = worldTransform_->GetRotate();
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
		obb.MakeOBBAxis(worldRotate);
		DrawOBB(obb, mat, Colors::Linear::green);
	} else if (emitter_.shape == (int)CpuEmitterShape::Shpere) {
		DrawSphere(emitter_.translate, emitter_.radius, mat, Colors::Linear::green);
	} else if (emitter_.shape == (int)CpuEmitterShape::Cone) {
		Math::Quaternion rotate = worldRotate;
		DrawCone(emitter_.translate, rotate, emitter_.radius, emitter_.angle * kToRadian, emitter_.height, mat);
	}
}

void AOENGINE::BaseParticles::Emit(const Math::Vector3& pos) {
	const uint32_t capacity = (std::max)(emitter_.maxParticles, 1u);
	if (particleArray_->size() >= capacity) {
		if (emitter_.overflowMode == static_cast<int>(ParticleOverflowMode::RecycleOldest)) {
			particleArray_->pop_front();
		} else {
			return;
		}
	}

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

	float conePhi = 0.0f;
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
		}
	}

	if (emitter_.shape == (int)CpuEmitterShape::Cone) {
		const float thickness = std::clamp(emitter_.radiusThickness, 0.0f, 1.0f);
		const float arcRadian = std::clamp(emitter_.arc, 0.0f, 360.0f) * kToRadian;
		conePhi = Random::RandomFloat(0.0f, arcRadian);
		const float coneAngle = std::clamp(emitter_.angle, 0.0f, 89.0f) * kToRadian;
		const float baseRadius = (std::max)(emitter_.radius, 0.0f);
		const float coneLength = (std::max)(emitter_.height, 0.0f);
		const float coneSlope = std::tan(coneAngle);
		float axialDistance = 0.0f;
		if (emitter_.coneEmitFrom == static_cast<int>(ConeEmitFrom::Volume) && coneLength > 0.0f) {
			if (coneSlope > 0.0001f) {
				const float endRadius = baseRadius + coneSlope * coneLength;
				const float volumeRadius = std::cbrt(Lerp(baseRadius * baseRadius * baseRadius,
					endRadius * endRadius * endRadius, Random::RandomFloat(0.0f, 1.0f)));
				axialDistance = (volumeRadius - baseRadius) / coneSlope;
			} else {
				axialDistance = Random::RandomFloat(0.0f, coneLength);
			}
		}
		const float sectionRadius = baseRadius + coneSlope * axialDistance;
		const float innerRadius = sectionRadius * (1.0f - thickness);
		const float radialRandom = Random::RandomFloat(0.0f, 1.0f);
		const float sampledRadius = std::sqrt(Lerp(innerRadius * innerRadius, sectionRadius * sectionRadius, radialRandom));
		const Math::Vector3 localPosition{
			std::cos(conePhi) * sampledRadius,
			axialDistance,
			std::sin(conePhi) * sampledRadius
		};
		newParticle.translate = pos + worldTransform_->GetWorldSRT().rotate * localPosition;
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
	Math::Quaternion worldRotate = worldTransform_->GetWorldSRT().rotate;
	if (emitter_.shape == (int)CpuEmitterShape::Cone) {
		const float coneAngle = std::clamp(emitter_.angle, 0.0f, 89.0f) * kToRadian;
		Math::Vector3 localDir{
			std::cos(conePhi) * std::sin(coneAngle),
			std::cos(coneAngle),
			std::sin(conePhi) * std::sin(coneAngle)
		};
		if (emitter_.randomDirectionAmount > 0.0f) {
			Math::Vector3 randomDir = Random::RandomVector3(CVector3::UNIT * -1.0f, CVector3::UNIT).Normalize();
			localDir = Math::Vector3::Lerp(localDir, randomDir,
				std::clamp(emitter_.randomDirectionAmount, 0.0f, 1.0f)).Normalize();
		}
		newParticle.velocity = localDir;
	}

	// Objectの回転に進行方向をあわせる
	Math::Vector3 dire = newParticle.velocity.Normalize();
	Math::Vector3 worldDire = worldRotate * dire;
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
	newParticle.blendModeType = emitter_.blendModeType;

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

	Math::Vector3 worldPos = worldTransform_->GetWorldPos();
	// 一度だけ打つフラグがtrueだったら
	if (!emitter_.isLoop) {
		for (uint32_t count = 0; count < emitter_.rateOverTimeCout; ++count) {
			Emit(worldPos);
		}
		isStop_ = true;
		return;
	}

	const Math::Vector3 movement = worldPos - preWorldPos_;
	const float distance = Length(movement);
	const float spacing = emitter_.emitSpacing > 0.001f ? emitter_.emitSpacing : 0.001f;
	const float deltaTime = AOENGINE::GameTimer::DeltaTime();
	emitAccumulator_ += emitter_.rateOverTimeCout * deltaTime;
	const uint32_t timeEmitCount = static_cast<uint32_t>(emitAccumulator_);
	emitAccumulator_ -= static_cast<float>(timeEmitCount);

	uint32_t distanceEmitCount = 0;
	const bool isTeleport = emitter_.teleportThreshold > 0.0f && distance > emitter_.teleportThreshold;
	const uint32_t frameLimit = (std::max)(emitter_.maxEmitPerFrame, 1u);
	if (distance > 0.0001f && !isTeleport) {
		const float firstDistance = spacing - distanceAccumulator_;
		distanceEmitCount = firstDistance <= distance
			? static_cast<uint32_t>(std::floor((distance - firstDistance) / spacing)) + 1
			: 0;

		const uint32_t emitCount = (std::min)((std::max)(timeEmitCount, distanceEmitCount), frameLimit);
		if (distanceEmitCount >= timeEmitCount) {
			for (uint32_t i = 0; i < emitCount; ++i) {
				const float emitDistance = firstDistance + spacing * static_cast<float>(i);
				Emit(Math::Vector3::Lerp(preWorldPos_, worldPos, emitDistance / distance));
			}
		} else {
			for (uint32_t i = 0; i < emitCount; ++i) {
				const float t = static_cast<float>(i + 1) / static_cast<float>(emitCount);
				Emit(Math::Vector3::Lerp(preWorldPos_, worldPos, t));
			}
		}
		distanceAccumulator_ = std::fmod(distanceAccumulator_ + distance, spacing);
	} else {
		const uint32_t emitCount = (std::min)(timeEmitCount, frameLimit);
		for (uint32_t i = 0; i < emitCount; ++i) {
			Emit(worldPos);
		}
		if (isTeleport) {
			distanceAccumulator_ = 0.0f;
		}
	}
	
	// 継続時間を進める
	currentTimer_ += deltaTime;
	if (currentTimer_ > emitter_.duration) {
		if (!emitter_.isLoop) {
			isStop_ = true;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ リセット処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::Reset() {
	emitAccumulator_ = 0.0f;
	distanceAccumulator_ = 0.0f;
	currentTimer_ = 0.0f;
	isStop_ = false;
	hasPreWorldPos_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseParticles::Debug_Gui() {
	InputTextWithString("Name.", "##cpuParticle", name_);
	emitter_.SetName(name_);
	ImGui::Separator();

	if (ImGui::Button("Reset")) {
		Reset();
	}

	ImGui::Checkbox("IsStop", &isStop_);
	emitter_.Attribute_Gui();

	ImGui::Separator();

	DrawTextureSelector();

	ImGui::Separator();
	ImGui::Text("マテリアル設定");
	shareMaterial_->Debug_Gui();

	meshName_ = AOENGINE::MeshManager::GetInstance()->SelectMeshName();
	if (ImGui::Button("ChangeMesh")) {
		changeMesh_ = true;
	}

	emitter_.SaveAndLoad();
}

void AOENGINE::BaseParticles::SetTexture(const std::string& textureName) {
	if (textureName.empty() || !AOENGINE::TextureManager::GetInstance()->ExistTexture(textureName)) {
		return;
	}

	emitter_.useTexture = textureName;
	if (shareMaterial_) {
		shareMaterial_->SetAlbedoTexture(textureName);
	}
}

void AOENGINE::BaseParticles::SetJsonData(const json& jsonData) {
	emitter_.FromJson(jsonData);
	const std::string meshName = emitter_.useMesh.empty() ? "plane" : emitter_.useMesh;
	if (auto mesh = AOENGINE::MeshManager::GetInstance()->GetMesh(meshName)) {
		shape_ = std::move(mesh);
	}
	blendModeType_ = emitter_.blendModeType;
	SetTexture(emitter_.useTexture);
	Reset();
}

void AOENGINE::BaseParticles::ClearParticles() {
	if (particleArray_) {
		particleArray_->clear();
	}
}

void AOENGINE::BaseParticles::DrawTextureSelector() {
	ImGui::Text("使用テクスチャ");
	AOENGINE::TextureManager* textureManager = AOENGINE::TextureManager::GetInstance();
	if (textureManager->ExistTexture(emitter_.useTexture)) {
		const D3D12_GPU_DESCRIPTOR_HANDLE handle = textureManager->GetDxHeapHandles(emitter_.useTexture).handleGPU;
		ImGui::Image(reinterpret_cast<ImTextureID>(handle.ptr), ImVec2(64.0f, 64.0f));
	} else {
		ImGui::Button("テクスチャなし", ImVec2(64.0f, 64.0f));
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_HANDLE")) {
			if (payload->Data != nullptr && payload->DataSize == sizeof(AssetHandle)) {
				const AssetHandle& assetHandle = *static_cast<const AssetHandle*>(payload->Data);
				if (assetHandle.type == AssetType::Texture) {
					SetTexture(textureManager->SearchSprite(assetHandle.id));
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("AssetsWindowからテクスチャをドロップしてください");
	}
	ImGui::SameLine();
	ImGui::TextWrapped("%s", emitter_.useTexture.c_str());
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

void AOENGINE::BaseParticles::SetParent(WorldTransform* parentTransform) {
	parentTransform_ = parentTransform;
}

void AOENGINE::BaseParticles::SetParentMatrix(const Math::Matrix4x4& parentMat) {
	parentMatrix_ = &parentMat;
}

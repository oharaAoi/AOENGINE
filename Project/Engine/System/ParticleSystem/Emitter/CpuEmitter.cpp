#include "CpuEmitter.h"
#include "Engine/Lib/GameTimer.h"
#include "imgui.h"

CpuEmitter::CpuEmitter() {}
CpuEmitter::~CpuEmitter() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CpuEmitter::Init() {
	// -------------------------------------------------
	// ↓ Emitterのz情報
	// -------------------------------------------------
	emitParameter_.rotate = Vector4();
	emitParameter_.translate = Vector3();
	emitParameter_.frequency = 0.5f;
	emitParameter_.frequencyTime = 0.0f;
	emitParameter_.count = 10;
	emitParameter_.emit = 0;

	// -------------------------------------------------
	// ↓ 生成するparticleの情報
	// -------------------------------------------------
	emitParameter_.color = Vector4(1, 1, 1, 1);
	emitParameter_.minScale = Vector3(1, 1, 1);
	emitParameter_.maxScale = Vector3(1, 1, 1);
	emitParameter_.speed = 1.0f;
	emitParameter_.lifeTime = 2.0f;
	emitParameter_.gravity = 0.0f;
	emitParameter_.dampig = 1.0f;

	// -------------------------------------------------
	// ↓ EmitterのAnimationの情報
	// -------------------------------------------------
	emitterParametr_.particleShape = "planeGeometry";
	emitterParametr_.materialTexture = "white.png";
	emitterParametr_.velocity = Vector3();
	emitterParametr_.speed = 1.0f;
	emitterParametr_.lifeTime = 1.0f;
	emitterParametr_.oneShot = false;

	isDead_ = false;
	isMove_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CpuEmitter::Update() {
	emitParameter_.frequencyTime += GameTimer::DeltaTime();

	// 間隔の時間を過ぎたら射出
	if (emitParameter_.frequencyTime >= emitParameter_.frequency) {
		// cout分生成する
		for (uint32_t index = 0; index < emitParameter_.count; ++index) {

		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CpuEmitter::DrawShape() {

}

#ifdef _DEBUG
void CpuEmitter::Debug_Gui() {
	int count = static_cast<int>(emitParameter_.count);

	ImGui::SliderFloat("frequencyTime", &emitParameter_.frequencyTime, 0.0f, emitParameter_.frequency);
	ImGui::DragFloat3("pos", &emitParameter_.translate.x, 0.1f);
	if (ImGui::InputInt("count", &count)) {
		if (count < 0) count = 0;
		emitParameter_.count = count;
	}
	ImGui::DragFloat("frequency", &emitParameter_.frequency, 0.1f);
}
#endif
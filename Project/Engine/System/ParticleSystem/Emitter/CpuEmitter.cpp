#include "CpuEmitter.h"

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
	emitterParametr_.particleModel = "cube.obj";
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

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CpuEmitter::DrawShape() {

}

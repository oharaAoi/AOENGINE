#include "Canvas2d.h"
#include "Engine.h"

Canvas2d::~Canvas2d() {
}

void Canvas2d::Init() {
	spriteList_.clear();
}

void Canvas2d::Update() {
	for (auto& sprite : spriteList_) {
		sprite->Update();
	}
}

void Canvas2d::Draw() const {
	Engine::SetPipeline(PSOType::Sprite, "Sprite_Normal.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	for (const auto& sprite : spriteList_) {
		if (sprite->GetEnable()) {
			sprite->Draw(pso);
		}
	}
}

void Canvas2d::DeleteSprite(Sprite* _sprite) {
	spriteList_.remove(_sprite);
}

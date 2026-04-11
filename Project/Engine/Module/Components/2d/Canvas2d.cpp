#include "Canvas2d.h"
#include "Engine/Core/Engine.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

AOENGINE::Canvas2d::~Canvas2d() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Init() {
	spriteList_.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Update() {
	// 生存確認
	for (auto it = spriteList_.begin(); it != spriteList_.end(); ) {
		if ((*it)->sprite->GetIsDestroy()) {
			it = spriteList_.erase(it);
		} else {
			++it;
		}
	}

	// ソートを行う
	spriteList_.sort([](const std::unique_ptr<ObjectPair>& a, const std::unique_ptr<ObjectPair>& b) {
		return a->sprite->GetRenderQueue() < b->sprite->GetRenderQueue();
					 });

	// 更新処理
	for (auto& it : spriteList_) {
		if (it->sprite->GetIsActive()) {
			it->sprite->Update();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Draw() const {
	for (const auto& it : spriteList_) {
		if (it->sprite->GetIsActive()) {
			Pipeline* pso = Engine::SetPipeline(PSOType::Sprite, it->psoName);
			it->sprite->Draw(pso);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::EditObject(const ImVec2& windowSize, const ImVec2& imagePos) {
	for (const auto& it : spriteList_) {
		if (it->sprite->GetIsActive()) {
			it->sprite->GetTransform()->Manipulate(windowSize, imagePos);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理 w
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Debug_Gui() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　追加処理
///////////////////////////////////////////////////////////da///////////////////////////////////////

Sprite* AOENGINE::Canvas2d::AddSprite(const std::string& textureName, const std::string& attributeName, const std::string& psoName, int renderQueue) {
	auto& newObj = spriteList_.emplace_back(std::make_unique<ObjectPair>());
	newObj->sprite = std::make_unique<Sprite>();
	newObj->sprite->Init(textureName);
	newObj->sprite->SetName(attributeName);
	newObj->sprite->SetRenderQueue(renderQueue);
	newObj->psoName = psoName;
	AddChild(newObj->sprite.get());
	return newObj->sprite.get();
}

AOENGINE::Canvas2d::ObjectPair* AOENGINE::Canvas2d::GetObjectPair(Sprite* _sprite) {
	for (const auto& it : spriteList_) {
		if (it->sprite.get() == _sprite) {
			return it.get();
		}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　リサイズ処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::ResizeSprite() {
	for (const auto& it : spriteList_) {
		it->sprite->Resize();
	}
}
#include "Canvas2d.h"
#include "Engine/Engine.h"

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
		return a->renderQueue < b->renderQueue;
					 });

	// 更新処理
	for (auto& it : spriteList_) {
		if (it->sprite->GetEnable()) {
			it->sprite->Update();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::PreDraw(const std::string& psoName) const {
	for (const auto& it : spriteList_) {
		if (it->isPreDraw) {
			if (it->sprite->GetEnable()) {
				Pipeline* pso = Engine::SetPipeline(PSOType::Sprite, psoName);
				it->sprite->Draw(pso);
			}
		}
	}
}

void AOENGINE::Canvas2d::Draw() const {
	for (const auto& it : spriteList_) {
		if (!it->isPreDraw) {
			if (it->sprite->GetEnable()) {
				Pipeline* pso = Engine::SetPipeline(PSOType::Sprite, it->psoName);
				it->sprite->Draw(pso);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::EditObject(const ImVec2& windowSize, const ImVec2& imagePos) {
	for (const auto& it : spriteList_) {
		if (it->sprite->GetEnable()) {
			it->sprite->GetTransform()->Manipulate(windowSize, imagePos);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　追加処理
//////////////////////////////////////////////////////////////////////////////////////////////////

Sprite* AOENGINE::Canvas2d::AddSprite(const std::string& _textureName, const std::string& _attributeName, const std::string& _psoName, int _renderQueue, bool _isPreDraw) {
	auto& newObj = spriteList_.emplace_back(std::make_unique<ObjectPair>());
	newObj->sprite = std::make_unique<Sprite>();
	newObj->sprite->Init(_textureName);
	newObj->sprite->SetName(_attributeName);
	newObj->psoName = _psoName;
	newObj->renderQueue = _renderQueue;
	newObj->isPreDraw = _isPreDraw;

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

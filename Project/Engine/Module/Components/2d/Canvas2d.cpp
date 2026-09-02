#include "Canvas2d.h"
#include "Engine/Core/Engine.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/Lib/Json/JsonSerializer.h"
#include "Engine/Lib/Path.h"

using namespace AOENGINE;

AOENGINE::Canvas2d::~Canvas2d() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Init() {
	isDestroy_ = false;
	itemHandles_.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Update() {
	RemoveInvalidItems();
	std::stable_sort(itemHandles_.begin(), itemHandles_.end(), [this](const ObjectHandle& a, const ObjectHandle& b) {
		const Sprite* spriteA = sceneWorld_ ? sceneWorld_->FindObjectAs<Sprite>(a) : nullptr;
		const Sprite* spriteB = sceneWorld_ ? sceneWorld_->FindObjectAs<Sprite>(b) : nullptr;
		return spriteA && spriteB && spriteA->GetRenderQueue() < spriteB->GetRenderQueue();
	});
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Draw() const {
	DrawForeground();
}

void AOENGINE::Canvas2d::DrawBackground() const {
	if (!sceneWorld_) { return; }
	for (const ObjectHandle& handle : itemHandles_) {
		const Sprite* sprite = sceneWorld_->FindObjectAs<Sprite>(handle);
		if (sprite && sprite->IsActive() && sprite->GetIsBackGround()) {
			const_cast<Sprite*>(sprite)->DrawSprite(true);
		}
	}
}

void AOENGINE::Canvas2d::DrawForeground() const {
	if (!sceneWorld_) { return; }
	for (const ObjectHandle& handle : itemHandles_) {
		const Sprite* sprite = sceneWorld_->FindObjectAs<Sprite>(handle);
		if (sprite && sprite->IsActive() && !sprite->GetIsBackGround()) {
			sprite->Draw();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::EditObject(const ImVec2& windowSize, const ImVec2& imagePos) {
	if (!sceneWorld_) { return; }
	for (const ObjectHandle& handle : itemHandles_) {
		Sprite* sprite = sceneWorld_->FindObjectAs<Sprite>(handle);
		if (sprite && sprite->IsActive()) {
			sprite->GetTransform()->Manipulate(windowSize, imagePos);
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
//////////////////////////////////////////////////////////////////////////////////////////////////

Sprite* AOENGINE::Canvas2d::AddSprite(const std::string& textureName, const std::string& attributeName, int renderQueue) {
	if (!sceneWorld_) { return nullptr; }
	auto ownedSprite = std::make_unique<Sprite>();
	Sprite* sprite = ownedSprite.get();
	if (!sceneWorld_->AddObject(std::move(ownedSprite), attributeName).IsValid()) { return nullptr; }
	sprite->Init(textureName);
	sprite->SetRenderQueue(renderQueue);
	itemHandles_.push_back(sprite->GetHandle());
	if (GetHandle().IsValid()) { sceneWorld_->SetParent(sprite->GetHandle(), GetHandle()); }
	return sprite;
}

Text* AOENGINE::Canvas2d::AddText(const std::string& attributeName, const std::string& text, int renderQueue) {
	if (!sceneWorld_) { return nullptr; }
	auto ownedText = std::make_unique<Text>();
	Text* newText = ownedText.get();
	if (!sceneWorld_->AddObject(std::move(ownedText), attributeName).IsValid()) { return nullptr; }
	newText->Init(text);
	newText->SetRenderQueue(renderQueue);
	itemHandles_.push_back(newText->GetHandle());
	if (GetHandle().IsValid()) { sceneWorld_->SetParent(newText->GetHandle(), GetHandle()); }
	return newText;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Spriteの取得
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::Sprite* AOENGINE::Canvas2d::GetSprite(const std::string& spriteName) {
	if (!sceneWorld_) { return nullptr; }
	for (const ObjectHandle& handle : itemHandles_) {
		Sprite* sprite = sceneWorld_->FindObjectAs<Sprite>(handle);
		if (sprite && sprite->GetName() == spriteName) {
			return sprite;
		}
	}
	return nullptr;
}

AOENGINE::Text* AOENGINE::Canvas2d::GetText(const std::string& textName) {
	if (!sceneWorld_) { return nullptr; }
	for (const ObjectHandle& handle : itemHandles_) {
		Text* text = sceneWorld_->FindObjectAs<Text>(handle);
		if (text && text->GetName() == textName) {
			return text;
		}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　リサイズ処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::ResizeSprite() {
	if (!sceneWorld_) { return; }
	for (const ObjectHandle& handle : itemHandles_) {
		if (Sprite* sprite = sceneWorld_->FindObjectAs<Sprite>(handle)) { sprite->Resize(); }
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Save(const std::string& sceneName) {
	json sceneData;
	if (!sceneWorld_) { return; }
	for (const ObjectHandle& handle : itemHandles_) {
		Sprite* sprite = sceneWorld_->FindObjectAs<Sprite>(handle);
		if (!sprite) { continue; }
		std::string spriteName = sprite->GetName();

		// dataの保存
		sprite->Save(sceneName + "_Canvas2d", spriteName);

		sceneData[sceneName].push_back({
			{"name", spriteName},
			{"type", sprite->GetCanvasItemType()}
									   });
	}

	bool result = AOENGINE::JsonSerializer::Save(kAssetPath + "/Game/GameData/JsonItems/" + sceneName + "/", "canvas",  sceneData);
	if (result) {
		AOENGINE::Logger::Log("Save_Canvas");
	} else {
		AOENGINE::Logger::AssertLog("Do Not Save Canvas");
	}
}

void AOENGINE::Canvas2d::RemoveInvalidItems() {
	if (!sceneWorld_) {
		itemHandles_.clear();
		return;
	}
	std::erase_if(itemHandles_, [this](const ObjectHandle& handle) {
		return !sceneWorld_->IsValid(handle);
	});
}

void AOENGINE::Canvas2d::AttachItemsToCanvas() {
	if (!sceneWorld_ || !GetHandle().IsValid()) { return; }
	RemoveInvalidItems();
	for (const ObjectHandle& handle : itemHandles_) {
		if (!sceneWorld_->GetParentHandle(handle).IsValid()) {
			sceneWorld_->SetParent(handle, GetHandle());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込みm処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Canvas2d::Load(const std::string& sceneName) {
	json sceneData = AOENGINE::JsonSerializer::Load(kAssetPath + "/Game/GameData/JsonItems/" + sceneName + "/", "canvas");

	if (sceneData.empty()) {
		return;
	}

	for (const auto& item : sceneData[sceneName]) {
		std::string name;
		std::string type = "Sprite";
		if (item.is_string()) {
			name = item.get<std::string>();
		} else {
			if (item.contains("name")) {
				name = item.at("name").get<std::string>();
			}
			if (item.contains("type")) {
				type = item.at("type").get<std::string>();
			}
		}
		if (name.empty()) {
			continue;
		}

		Sprite* sprite = nullptr;
		if (type == "Text") {
			sprite = AddText(name);
		} else {
			sprite = AddSprite("white.png", name);
		}
		sprite->Load(sceneName + "_Canvas2d", name);
	}
}

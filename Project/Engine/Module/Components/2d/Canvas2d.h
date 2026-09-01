#pragma once
#include <vector>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/2d/Text.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/System/Scene/SceneWorld.h"

namespace AOENGINE {

/// <summary>
/// Spriteをまとめたクラス
/// </summary>
class Canvas2d :
	public ISceneObject {
public: // constructor

	Canvas2d() = default;
	~Canvas2d();
	ScenePersistence GetScenePersistence() const override { return ScenePersistence::SystemObject; }

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	void PostUpdate() override {}
	void PreDraw() const override {}

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const override;
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) override { EditObject(windowSize, imagePos); }

	/// <summary>
	/// spriteの編集する
	/// </summary>
	/// <param name="windowSize"></param>
	/// <param name="imagePos"></param>
	void EditObject(const ImVec2& windowSize, const ImVec2& imagePos);

	/// <summary>
	/// Spriteをリサイズする
	/// </summary>
	void ResizeSprite();

	/// <summary>
	/// 編集項目
	/// </summary>
	void Debug_Gui();

	/// <summary>
	/// 保存処理
	/// </summary>
	/// <param name="sceneName"></param>
	void Save(const std::string& sceneName);

	/// <summary>
	/// 読み込み処理
	/// </summary>
	/// <param name="sceneName"></param>
	void Load(const std::string& sceneName);

	/// <summary>
	/// Spriteの追加
	/// </summary>
	/// <param name="textureName">: Textureの名前</param>
	/// <param name="attributeName">: Spriteの固有名</param>
	/// <param name="renderQueue">: レンダリングする順番</param>
	/// <returns></returns>
	AOENGINE::Sprite* AddSprite(const std::string& textureName, const std::string& attributeName, int renderQueue = 0);
	AOENGINE::Text* AddText(const std::string& attributeName, const std::string& text = "New Text", int renderQueue = 0);

public: // accessor

	AOENGINE::Sprite* GetSprite(const std::string& spriteName);
	AOENGINE::Text* GetText(const std::string& textName);
	void SetSceneWorld(SceneWorld* sceneWorld) { sceneWorld_ = sceneWorld; }
	void AttachItemsToCanvas();

private: // private variable

	void RemoveInvalidItems();
	std::vector<ObjectHandle> itemHandles_;
	SceneWorld* sceneWorld_ = nullptr;

};
}

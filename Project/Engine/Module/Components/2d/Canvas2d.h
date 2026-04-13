#pragma once
#include <list>
#include <memory>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AOENGINE {

/// <summary>
/// Spriteをまとめたクラス
/// </summary>
class Canvas2d :
	public AttributeGui {
public: // constructor

	Canvas2d() = default;
	~Canvas2d();

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

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
	void Debug_Gui() override;

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

public: // accessor

	AOENGINE::Sprite* GetSprite(const std::string& spriteName);

private: // private variable

	std::list<std::unique_ptr<AOENGINE::Sprite>> spriteList_;

};
}
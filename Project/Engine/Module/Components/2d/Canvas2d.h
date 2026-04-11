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
public:

	/// <summary>
	/// Spriteを含めた情報
	/// </summary>
	struct ObjectPair {
		std::unique_ptr<AOENGINE::Sprite> sprite;
		std::string psoName;
	};

public: // コンストラクタ

	Canvas2d() = default;
	~Canvas2d();

public:

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

public:

	AOENGINE::Sprite* AddSprite(const std::string& textureName, const std::string& attributeName, const std::string& psoName = "Sprite_Normal.json", int renderQueue = 0);

	ObjectPair* GetObjectPair(Sprite* sprite);

private:

	std::list<std::unique_ptr<ObjectPair>> spriteList_;

};
}
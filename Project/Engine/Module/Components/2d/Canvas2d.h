#pragma once
#include <list>
#include <memory>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"

namespace AOENGINE {

/// <summary>
/// Spriteをまとめたクラス
/// </summary>
class Canvas2d {
public:

	/// <summary>
	/// Spriteを含めた情報
	/// </summary>
	struct ObjectPair {
		std::unique_ptr<AOENGINE::Sprite> sprite;
		std::string psoName;
		int renderQueue = 0;
		bool isPreDraw = false;
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
	/// 背景描画
	/// </summary>
	/// <param name="psoName"></param>
	void PreDraw(const std::string& psoName = "Sprite_Normal_16.json") const;

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

public:

	AOENGINE::Sprite* AddSprite(const std::string& _textureName, const std::string& _attributeName, const std::string& _psoName = "Sprite_Normal.json", int _renderQueue = 0, bool _isPreDraw = false);

	ObjectPair* GetObjectPair(Sprite* _sprite);

private:

	std::list<std::unique_ptr<ObjectPair>> spriteList_;

};
}
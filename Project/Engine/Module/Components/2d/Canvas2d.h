#pragma once

#include <list>
#include <memory>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"

class Canvas2d {
public:

	struct ObjectPair {
		std::unique_ptr<Sprite> sprite;
		std::string psoName;
		int renderQueue = 0;
		bool isPreDraw = false;
	};

public:

	Canvas2d() = default;
	~Canvas2d();

	void Init();

	void Update();

	void PreDraw(const std::string& psoName = "Sprite_Normal_16.json") const;

	void Draw() const;

	void EditObject(const ImVec2& windowSize, const ImVec2& imagePos);

public:

	Sprite* AddSprite(const std::string& _textureName, const std::string& _attributeName, const std::string& _psoName = "Sprite_Normal.json", int _renderQueue = 0, bool _isPreDraw = false);

	ObjectPair* GetObjectPair(Sprite* _sprite);

private:

	std::list<std::unique_ptr<ObjectPair>> spriteList_;

};




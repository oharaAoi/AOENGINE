#pragma once

#include "Engine/Module/PostEffect/PostProcess.h"

namespace AOENGINE {

class PostProcessInspector final {
public:
	static void Draw(PostProcess& postProcess);
};

class PostEffectInspector final {
public:
	static void Draw(PostEffectType type, PostEffect::IPostEffect& effect);
};

}

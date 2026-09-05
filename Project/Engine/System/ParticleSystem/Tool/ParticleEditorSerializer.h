#pragma once
#include <string>
#include <vector>
#include "Engine/System/ParticleSystem/Effect/ParticleEffect.h"

/// <summary>
/// ParticleEditorから保存を行うクラス
/// </summary>
class ParticleEditorSerializer {
public:
	static bool SaveComposite(const AOENGINE::ParticleEffectAsset& asset);
	static AOENGINE::ParticleEffectAsset LoadComposite(const std::string& name);
};


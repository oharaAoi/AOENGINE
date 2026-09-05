#include "ParticleEditorSerializer.h"
#include "Engine/Lib/Json/JsonItems.h"

bool ParticleEditorSerializer::SaveComposite(const AOENGINE::ParticleEffectAsset& asset) {
	if (asset.name.empty() || asset.nodes.empty()) { return false; }
	AOENGINE::JsonItems::Save("Composite", asset.ToJson(), AOENGINE::JsonItems::GetDirectoryPath(), "Effect");
	// 保存直後からPlayできるよう、JsonItemsのキャッシュにも反映する。
	AOENGINE::JsonItems::Load("Composite", asset.name, AOENGINE::JsonItems::GetDirectoryPath(), "Effect");
	return true;
}

AOENGINE::ParticleEffectAsset ParticleEditorSerializer::LoadComposite(const std::string& name) {
	return AOENGINE::ParticleEffectAsset::FromJson(AOENGINE::JsonItems::GetData("Composite", name), name);
}

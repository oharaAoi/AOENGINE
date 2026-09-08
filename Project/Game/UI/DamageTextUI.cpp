#include "DamageTextUI.h"

/// stl
#include <algorithm>
#include <cstdio>

namespace {
	/// 生成に使うPrefab名
	const std::string kPrefabName = "DamageText";
}

bool DamageTextUI::Spawn(const std::string& text,const Math::Vector3& position,const DamageTextUIParameter& parameter){
	return SpawnText(kPrefabName,text,position,parameter.popup);
}

std::string DamageTextUI::MakeCountText(int blockCount,int comboCount,const DamageTextUIParameter& parameter){
	return std::to_string(blockCount) + parameter.blockLabel
		+ parameter.countSeparator
		+ std::to_string(comboCount) + parameter.comboLabel;
}

std::string DamageTextUI::MakeDamageText(float damage,const DamageTextUIParameter& parameter){
	// 桁数は調整値から来るため、書式に渡す前にここで丸めておく
	const int decimals = std::clamp(static_cast<int>(parameter.decimals),0,3);

	char buffer[32]{};
	std::snprintf(buffer,sizeof(buffer),"%.*f",decimals,damage);

	return parameter.textPrefix + std::string(buffer) + parameter.textSuffix;
}

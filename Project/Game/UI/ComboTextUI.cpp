#include "ComboTextUI.h"

/// stl
#include <string>

namespace {
	/// 生成に使うPrefab名
	const std::string kPrefabName = "ComboText";
}

bool ComboTextUI::Spawn(int comboCount,const Math::Vector3& position,const ComboTextUIParameter& parameter){
	return SpawnText(kPrefabName,
					 std::to_string(comboCount) + parameter.textSuffix,
					 position,
					 parameter.popup);
}

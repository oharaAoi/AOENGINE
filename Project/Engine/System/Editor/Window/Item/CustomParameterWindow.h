#pragma once

#include "Engine/System/Editor/Window/Item/IWindowItem.h"

namespace AOENGINE {

class CustomParameterWindow final : public IWindowItem {
public:
	void Init() override;
	void Edit() override;
};

}

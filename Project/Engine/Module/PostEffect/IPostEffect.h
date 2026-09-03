#pragma once
#include <string>
#include "Engine/DirectX/Resource/DxResource.h"

namespace AOENGINE { class IJsonConverter; }

namespace AOENGINE {
	class PostProcess;
}

namespace PostEffect {

/// <summary>
/// PostEffectの基底クラス
/// </summary>
class IPostEffect {
public:

	IPostEffect() = default;
	virtual ~IPostEffect() = default;

	virtual void Init() = 0;

	virtual void PostInit(AOENGINE::PostProcess* ) {}

	virtual void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) = 0;

	virtual void ApplySaveSettings() = 0;

	virtual void Save(const std::string& rootField) = 0;

	virtual void Load(const std::string& rootField) = 0;

	// SceneSerializer向け。ファイルI/OはPostProcess側へ集約する。
	virtual AOENGINE::IJsonConverter* GetSettingsConverter() = 0;
	virtual const AOENGINE::IJsonConverter* GetSettingsConverter() const = 0;

public:

	void SetIsEnable(bool _isEnable) { isEnable_ = _isEnable; }
	bool GetIsEnable() const { return isEnable_; }

protected:

	bool isEnable_ = false;

};

}

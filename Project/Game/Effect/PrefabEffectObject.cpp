#include "PrefabEffectObject.h"

/// engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Utilities/Logger.h"

PrefabEffectObject::PrefabEffectObject(const std::string& prefabName){
	AOENGINE::SceneObject* root = AOENGINE::PrefabManager::GetInstance()->Instantiate(prefabName);
	AOENGINE::BaseGameObject* gameObject = dynamic_cast<AOENGINE::BaseGameObject*>(root);
	if(gameObject == nullptr){
		// 生成に失敗した場合は Bind() されないため IsValid() が false のままになり、
		// 以降の SetParent() 等の操作は全て何もせず空振りする
		AOENGINE::Logger::Log("[EffectObject][Prefab] Instantiate failed: " + prefabName + "\n");
		return;
	}
	Bind(gameObject);

	// Play() されるまでは画面に出したくないため、生成直後は非表示にしておく
	SetIsActive(false);
}

PrefabEffectObject::~PrefabEffectObject(){
	// 親(EffectObjectGroup、ひいては Launcher)が消える時にここが呼ばれる。
	// SceneWorld から実体と子階層を削除することでリークを残さない
	Destroy();
}

void PrefabEffectObject::SetParent(AOENGINE::WorldTransform* parent){
	AOENGINE::WorldTransform* transform = GetTransform();
	if(transform == nullptr){
		return;
	}

	if(parent == nullptr){
		transform->ClearParent();
		return;
	}

	// WorldTransform は親の行列をポインタとして持ち続ける(parentWorldMat_ = &parentMat)ため、
	// 親は自分より長生きしている必要がある
	transform->SetParent(parent->GetWorldMatrix());
}

void PrefabEffectObject::SetLocalPosition(const Math::Vector3& position){
	AOENGINE::WorldTransform* transform = GetTransform();
	if(transform == nullptr){
		return;
	}
	transform->SetTranslate(position);
}

void PrefabEffectObject::Play(){
	SetIsActive(true);
}

void PrefabEffectObject::Stop(){
	SetIsActive(false);
}

bool PrefabEffectObject::IsAlive() const{
	return IsValid();
}

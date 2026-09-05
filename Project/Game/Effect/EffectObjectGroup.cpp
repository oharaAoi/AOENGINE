#include "EffectObjectGroup.h"

/// engine
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/ParticleSystem/Effect/ParticleEffect.h"
#include "Engine/Utilities/Logger.h"

/// game
#include "Game/Effect/IEffectObject.h"

EffectObjectGroup::~EffectObjectGroup(){
	Destroy();
}

IEffectObject* EffectObjectGroup::Add(std::unique_ptr<IEffectObject> object){
	if(object == nullptr){
		return nullptr;
	}

	Entry entry;
	entry.kind = Entry::Kind::External;
	IEffectObject* result = object.get();
	entry.object = std::move(object);
	objects_.push_back(std::move(entry));
	return result;
}

ParticleEffectObject* EffectObjectGroup::AddParticle(const std::string& particleName,const Math::Vector3& localPosition,float delay){
	auto particleObject = std::make_unique<ParticleEffectObject>(particleName);
	ParticleEffectObject* result = particleObject.get();
	result->SetLocalPosition(localPosition);
	result->SetDelay(delay);

	Entry entry;
	entry.kind = Entry::Kind::Particle;
	entry.name = particleName;
	entry.localPosition = localPosition;
	entry.delay = delay;
	entry.object = std::move(particleObject);
	objects_.push_back(std::move(entry));
	return result;
}

size_t EffectObjectGroup::AddParticleEffect(const std::string& effectName,const Math::Vector3& localPosition){
	// JsonItems::GetData() は見つからないと例外を投げるため、
	// ParticleEffectManager::Play() と同じように握りつぶして扱う
	AOENGINE::ParticleEffectAsset asset;
	try{
		asset = AOENGINE::ParticleEffectAsset::FromJson(
			AOENGINE::JsonItems::GetData("Composite",effectName),effectName);
	} catch(...){
		AOENGINE::Logger::Log("[EffectObject][Effect] Load failed: " + effectName + "\n");
		return 0;
	}

	size_t addedCount = 0;
	for(const AOENGINE::ParticleEffectNode& node : asset.nodes){
		// GPU Particle を包む演出オブジェクトはまだ無いので、CPU のノードだけを扱う
		if(node.type != AOENGINE::ParticleEffectNodeType::Cpu){
			AOENGINE::Logger::Log("[EffectObject][Effect] GPU node skipped: " + effectName + " / " + node.name + "\n");
			continue;
		}

		// ノードの位置は複合エフェクトの原点からの相対位置なので、指定されたローカル座標へ足す
		AddParticle(node.asset,localPosition + node.position,node.delay);
		++addedCount;
	}

	return addedCount;
}

PrefabEffectObject* EffectObjectGroup::AddPrefab(const std::string& prefabName,const Math::Vector3& localPosition){
	auto prefabObject = std::make_unique<PrefabEffectObject>(prefabName);
	PrefabEffectObject* result = prefabObject.get();
	result->SetLocalPosition(localPosition);

	Entry entry;
	entry.kind = Entry::Kind::Prefab;
	entry.name = prefabName;
	entry.localPosition = localPosition;
	entry.object = std::move(prefabObject);
	objects_.push_back(std::move(entry));
	return result;
}

void EffectObjectGroup::SetParent(AOENGINE::WorldTransform* parent){
	pParent_ = parent;

	for(Entry& entry : objects_){
		if(entry.object == nullptr){
			continue;
		}
		entry.object->SetParent(parent);
	}
}

void EffectObjectGroup::Play(){
	// PrefabEffectObject が指す GameObject は SceneWorld のクリアやエディタ操作で
	// こちらの与り知らないところで破棄されうるため、再生の直前に生存確認して作り直す
	RebuildDeadObjects();

	for(Entry& entry : objects_){
		if(entry.object == nullptr){
			continue;
		}
		entry.object->Play();
	}
}

void EffectObjectGroup::Stop(){
	for(Entry& entry : objects_){
		if(entry.object == nullptr){
			continue;
		}
		entry.object->Stop();
	}
}

void EffectObjectGroup::Update(float deltaTime){
	for(Entry& entry : objects_){
		if(entry.object == nullptr){
			continue;
		}
		entry.object->Update(deltaTime);
	}
}

void EffectObjectGroup::Destroy(){
	objects_.clear();
}

void EffectObjectGroup::RebuildDeadObjects(){
	for(Entry& entry : objects_){
		if(entry.object != nullptr && entry.object->IsAlive()){
			continue;
		}

		// 外から Add() された物は作り方が分からないので作り直せない
		if(entry.kind == Entry::Kind::External){
			continue;
		}

		if(entry.kind == Entry::Kind::Particle){
			auto particleObject = std::make_unique<ParticleEffectObject>(entry.name);
			// delay は Particle だけが持つ概念なので、作り直したここで設定し直す
			particleObject->SetDelay(entry.delay);
			entry.object = std::move(particleObject);
		} else{
			entry.object = std::make_unique<PrefabEffectObject>(entry.name);
		}

		// 作り直した物には、今の親とローカル座標を設定し直す
		entry.object->SetParent(pParent_);
		entry.object->SetLocalPosition(entry.localPosition);
	}
}

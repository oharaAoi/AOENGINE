#include "SceneSerializer.h"

#include <unordered_map>
#include <nlohmann/json.hpp>

#include "Engine/Lib/Json/JsonSerializer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/GameObject/SceneObject.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/System/Scene/SceneObjectSerializer.h"
#include "Engine/Module/PostEffect/PostProcess.h"

using json = nlohmann::json;
using namespace AOENGINE;

namespace {

json Vector3ToJson(const Math::Vector3& value) {
	return json::array({ value.x, value.y, value.z });
}

json QuaternionToJson(const Math::Quaternion& value) {
	return json::array({ value.x, value.y, value.z, value.w });
}

Math::Vector3 JsonToVector3(const json& value) {
	return { value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>() };
}

Math::Quaternion JsonToQuaternion(const json& value) {
	return { value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>(), value.at(3).get<float>() };
}

bool IsChildOfPrefabInstance(const SceneObject& object, const SceneWorld& world) {
	ObjectHandle parentHandle = world.GetParentHandle(object.GetHandle());
	while (const SceneObject* parent = world.FindObject(parentHandle)) {
		if (parent->IsPrefabInstanceRoot()) { return true; }
		parentHandle = world.GetParentHandle(parentHandle);
	}
	return false;
}

json SerializePrefabInstance(const SceneObject& object) {
	json data = { { "prefab", object.GetPrefabSource() } };
	if (const auto* gameObject = dynamic_cast<const BaseGameObject*>(&object)) {
		data["isReflection"] = gameObject->GetIsReflection();
		data["isRendering"] = gameObject->GetIsRendering();
		data["enableShadow"] = gameObject->GetEnableShadow();
		if (gameObject->GetTransform()) {
			const Math::QuaternionSRT& srt = gameObject->GetTransform()->GetSRT();
			data["transformType"] = "3D";
			data["translate"] = Vector3ToJson(srt.translate);
			data["rotate"] = QuaternionToJson(srt.rotate);
			data["scale"] = Vector3ToJson(srt.scale);
		}
	} else if (const auto* sprite = dynamic_cast<const Sprite*>(&object)) {
		const Math::SRT& srt = sprite->GetTransform()->GetTransform();
		data["transformType"] = "2D";
		data["translate"] = Vector3ToJson(srt.translate);
		data["rotate"] = Vector3ToJson(srt.rotate);
		data["scale"] = Vector3ToJson(srt.scale);
	}
	return data;
}

void ApplyPrefabInstanceTransform(SceneObject& object, const json& data) {
	if (!data.contains("translate") || !data.contains("rotate") || !data.contains("scale")) { return; }
	if (data.value("transformType", "") == "3D") {
		if (auto* gameObject = dynamic_cast<BaseGameObject*>(&object); gameObject && gameObject->GetTransform()) {
			// Prefab参照をシーンへ保存した場合のインスタンス設定を復元する。
			gameObject->SetIsReflection(data.value("isReflection", gameObject->GetIsReflection()));
			gameObject->SetIsRendering(data.value("isRendering", gameObject->GetIsRendering()));
			gameObject->SetEnableShadow(data.value("enableShadow", gameObject->GetEnableShadow()));
			Math::QuaternionSRT srt{};
			srt.translate = JsonToVector3(data.at("translate"));
			srt.rotate = JsonToQuaternion(data.at("rotate"));
			srt.scale = JsonToVector3(data.at("scale"));
			gameObject->GetTransform()->SetSRT(srt);
		}
	} else if (data.value("transformType", "") == "2D") {
		if (auto* sprite = dynamic_cast<Sprite*>(&object)) {
			Math::SRT srt{};
			srt.translate = JsonToVector3(data.at("translate"));
			srt.rotate = JsonToVector3(data.at("rotate"));
			srt.scale = JsonToVector3(data.at("scale"));
			sprite->GetTransform()->SetSRT(srt);
		}
	}
}

}

json SceneSerializer::Serialize(const std::string& sceneName, const SceneRenderer& renderer,
	const PostProcess& postProcess) {
	json root = {
		{ "format", "AOENGINE_SCENE" },
		{ "version", 2 },
		{ "sceneName", sceneName },
		{ "environment", { { "postProcess", postProcess.Serialize() } } },
		{ "objects", json::array() }
	};

	const SceneWorld& world = renderer.GetSceneWorld();
	for (const ObjectHandle& handle : world.GetObjectHandles()) {
		const SceneObject* object = world.FindObject(handle);
		if (!object || object->GetScenePersistence() != ScenePersistence::SceneData) { continue; }
		if (IsChildOfPrefabInstance(*object, world)) { continue; }

		json objectJson = {
			{ "id", object->GetSceneId() },
			{ "type", object->IsPrefabInstanceRoot() ? "PrefabInstance" : object->GetSceneTypeName() },
			{ "name", object->GetName() },
			{ "active", object->IsActive() },
			{ "data", object->IsPrefabInstanceRoot()
				? SerializePrefabInstance(*object) : SceneObjectSerializer::Serialize(*object) }
		};
		const ObjectHandle parentHandle = world.GetParentHandle(handle);
		const SceneObject* parent = world.FindObject(parentHandle);
		objectJson["parentId"] = parent && parent->GetScenePersistence() == ScenePersistence::SceneData
			? json(parent->GetSceneId()) : json(nullptr);
		root["objects"].push_back(std::move(objectJson));
	}

	return root;
}

bool SceneSerializer::Deserialize(const json& root, SceneRenderer& renderer, Canvas2d& canvas,
	PostProcess& postProcess) {
	if (!root.is_object()) { return false; }
	const int version = root.value("version", 0);
	if (root.value("format", "") != "AOENGINE_SCENE" ||
		(version != 1 && version != 2)) {
		return false;
	}

	postProcess.ResetToDefaults();
	if (root.contains("environment") && root.at("environment").is_object()) {
		const json& environment = root.at("environment");
		if (environment.contains("postProcess") && !postProcess.Deserialize(environment.at("postProcess"))) {
			return false;
		}
	}

	canvas.SetSceneWorld(&renderer.GetSceneWorld());
	for (const ObjectHandle& handle : renderer.GetObjectHandles()) {
		const SceneObject* object = renderer.FindObject(handle);
		if (object && object->GetScenePersistence() == ScenePersistence::SceneData) {
			renderer.DestroyObject(handle);
		}
	}
	canvas.Update();

	std::unordered_map<uint64_t, ObjectHandle> loadedObjects;
	for (const json& objectJson : root.at("objects")) {
		SceneObject* object = nullptr;
		if (objectJson.value("type", "") == "PrefabInstance") {
			const json& data = objectJson.at("data");
			object = PrefabManager::GetInstance()->Instantiate(
				data.value("prefab", ""), renderer, canvas);
			if (object) { ApplyPrefabInstanceTransform(*object, data); }
		} else {
			object = SceneObjectSerializer::Deserialize(objectJson, renderer, canvas);
		}
		if (!object) { continue; }
		object->SetName(objectJson.value("name", object->GetName()));
		const uint64_t id = objectJson.at("id").get<uint64_t>();
		object->SetSceneId(id);
		object->SetActive(objectJson.value("active", true));
		loadedObjects[id] = object->GetHandle();
	}

	for (const json& objectJson : root.at("objects")) {
		if (objectJson.at("parentId").is_null()) { continue; }
		const uint64_t id = objectJson.at("id").get<uint64_t>();
		const uint64_t parentId = objectJson.at("parentId").get<uint64_t>();
		if (loadedObjects.contains(id) && loadedObjects.contains(parentId)) {
			renderer.SetParent(loadedObjects.at(id), loadedObjects.at(parentId));
		}
	}
	canvas.AttachItemsToCanvas();
	return true;
}

bool SceneSerializer::Save(const std::string& folderPath, const std::string& fileName,
	const SceneRenderer& renderer, const PostProcess& postProcess) {
	return JsonSerializer::Save(folderPath, fileName, Serialize(fileName, renderer, postProcess));
}

bool SceneSerializer::Load(const std::string& folderPath, const std::string& fileName,
	SceneRenderer& renderer, Canvas2d& canvas, PostProcess& postProcess) {
	const json root = JsonSerializer::Load(folderPath, fileName);
	const bool hasEmbeddedSettings = root.contains("environment") &&
		root.at("environment").is_object() && root.at("environment").contains("postProcess");
	if (!Deserialize(root, renderer, canvas, postProcess)) { return false; }
	if (!hasEmbeddedSettings) {
		// version 1のシーンは従来ファイルを一度読み、次回保存時に埋め込み形式へ移行する。
		JsonItems::GetInstance()->LoadDesignationPath(folderPath);
		postProcess.Load(fileName + "_");
	}
	return true;
}

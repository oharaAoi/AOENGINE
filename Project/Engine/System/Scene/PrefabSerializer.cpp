#include "PrefabSerializer.h"

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

#include "Engine/Lib/Json/JsonSerializer.h"
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneObjectSerializer.h"

using json = nlohmann::json;
using namespace AOENGINE;

namespace {

void CollectHierarchy(const SceneObject& object, const SceneWorld& world,
	std::vector<const SceneObject*>& objects) {
	objects.push_back(&object);
	for (const SceneObject* child : object.GetChildren()) {
		if (!child || world.FindObject(child->GetHandle()) != child) { continue; }
		CollectHierarchy(*child, world, objects);
	}
}

void DestroyCreatedObjects(SceneRenderer& renderer, const std::vector<ObjectHandle>& handles) {
	for (auto it = handles.rbegin(); it != handles.rend(); ++it) {
		if (renderer.FindObject(*it)) {
			renderer.DestroyObject(*it);
		}
	}
}

}

json PrefabSerializer::Serialize(
	const std::string& prefabName, ObjectHandle rootHandle, const SceneRenderer& renderer) {
	const SceneWorld& world = renderer.GetSceneWorld();
	const SceneObject* prefabRoot = world.FindObject(rootHandle);
	if (!prefabRoot) { return json(); }

	std::vector<const SceneObject*> objects;
	CollectHierarchy(*prefabRoot, world, objects);

	std::unordered_map<const SceneObject*, uint64_t> localIds;
	for (size_t index = 0; index < objects.size(); ++index) {
		localIds[objects[index]] = static_cast<uint64_t>(index + 1);
	}

	json prefab = {
		{ "format", "AOENGINE_PREFAB" },
		{ "version", 1 },
		{ "prefabName", prefabName },
		{ "rootId", localIds.at(prefabRoot) },
		{ "objects", json::array() }
	};

	for (const SceneObject* object : objects) {
		const ObjectHandle parentHandle = world.GetParentHandle(object->GetHandle());
		const SceneObject* parent = world.FindObject(parentHandle);
		const auto parentIt = localIds.find(parent);

		prefab["objects"].push_back({
			{ "id", localIds.at(object) },
			{ "parentId", parentIt != localIds.end() ? json(parentIt->second) : json(nullptr) },
			{ "type", object->GetSceneTypeName() },
			{ "name", object->GetName() },
			{ "active", object->IsActive() },
			{ "data", SceneObjectSerializer::Serialize(*object) }
		});
	}

	return prefab;
}

SceneObject* PrefabSerializer::Deserialize(
	const json& root, SceneRenderer& renderer, Canvas2d& canvas) {
	if (!root.is_object() || root.value("format", "") != "AOENGINE_PREFAB" ||
		root.value("version", 0) != 1 || !root.contains("objects") ||
		!root.at("objects").is_array() || !root.contains("rootId")) {
		return nullptr;
	}

	canvas.SetSceneWorld(&renderer.GetSceneWorld());
	std::unordered_map<uint64_t, ObjectHandle> loadedObjects;
	std::vector<ObjectHandle> createdHandles;

	for (const json& objectJson : root.at("objects")) {
		if (!objectJson.contains("id")) {
			DestroyCreatedObjects(renderer, createdHandles);
			return nullptr;
		}

		SceneObject* object = SceneObjectSerializer::Deserialize(objectJson, renderer, canvas);
		if (!object) {
			DestroyCreatedObjects(renderer, createdHandles);
			canvas.Update();
			return nullptr;
		}

		const uint64_t localId = objectJson.at("id").get<uint64_t>();
		if (loadedObjects.contains(localId)) {
			createdHandles.push_back(object->GetHandle());
			DestroyCreatedObjects(renderer, createdHandles);
			canvas.Update();
			return nullptr;
		}

		object->SetActive(objectJson.value("active", true));
		loadedObjects[localId] = object->GetHandle();
		createdHandles.push_back(object->GetHandle());
	}

	for (const json& objectJson : root.at("objects")) {
		if (!objectJson.contains("parentId") || objectJson.at("parentId").is_null()) { continue; }
		const uint64_t localId = objectJson.at("id").get<uint64_t>();
		const uint64_t parentId = objectJson.at("parentId").get<uint64_t>();
		if (!loadedObjects.contains(localId) || !loadedObjects.contains(parentId) ||
			!renderer.SetParent(loadedObjects.at(localId), loadedObjects.at(parentId))) {
			DestroyCreatedObjects(renderer, createdHandles);
			canvas.Update();
			return nullptr;
		}
	}

	const uint64_t rootId = root.at("rootId").get<uint64_t>();
	if (!loadedObjects.contains(rootId)) {
		DestroyCreatedObjects(renderer, createdHandles);
		canvas.Update();
		return nullptr;
	}

	canvas.AttachItemsToCanvas();
	SceneObject* prefabRoot = renderer.FindObject(loadedObjects.at(rootId));
	if (prefabRoot) {
		prefabRoot->SetPrefabSource(root.value("prefabName", ""));
	}
	return prefabRoot;
}

bool PrefabSerializer::Save(const std::string& folderPath, const std::string& prefabName,
	ObjectHandle rootHandle, const SceneRenderer& renderer) {
	const json prefab = Serialize(prefabName, rootHandle, renderer);
	if (prefab.is_null()) { return false; }
	return JsonSerializer::Save(folderPath, prefabName + ".prefab", prefab);
}

SceneObject* PrefabSerializer::Load(const std::string& folderPath, const std::string& prefabName,
	SceneRenderer& renderer, Canvas2d& canvas) {
	return Deserialize(JsonSerializer::Load(folderPath, prefabName + ".prefab"), renderer, canvas);
}

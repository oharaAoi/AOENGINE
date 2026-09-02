#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace AOENGINE {

enum class ScenePersistence {
	SceneData,
	SystemObject,
	RuntimeOnly
};

static constexpr uint32_t kInvalidObjectHandleIndex = UINT32_MAX;

struct ObjectHandle {
	uint32_t index = kInvalidObjectHandleIndex;
	uint32_t generation = 0;

	bool IsValid() const { return index != kInvalidObjectHandleIndex && generation != 0; }
	bool operator==(const ObjectHandle& other) const = default;
};

class SceneObject {
public:
	virtual ~SceneObject() = default;
	virtual ScenePersistence GetScenePersistence() const { return ScenePersistence::RuntimeOnly; }
	virtual const char* GetSceneTypeName() const { return "SceneObject"; }

	uint64_t GetSceneId() const;
	void SetSceneId(uint64_t sceneId);

	ObjectHandle GetHandle() const;
	void SetHandle(const ObjectHandle& handle);

	const std::string& GetName() const;
	void SetName(const std::string& name);

	bool IsActive() const;
	void SetActive(bool active);

	/// <summary>このオブジェクトをPrefabインスタンスのルートとして設定する。</summary>
	void SetPrefabSource(const std::string& prefabName);
	void ClearPrefabSource();
	bool IsPrefabInstanceRoot() const { return !prefabSource_.empty(); }
	const std::string& GetPrefabSource() const { return prefabSource_; }

	void AddChild(SceneObject* child);
	void DeleteChild(SceneObject* child);
	bool HasChild() const { return !children_.empty(); }
	const std::vector<SceneObject*>& GetChildren() const { return children_; }

private:
	ObjectHandle handle_;
	uint64_t sceneId_ = 0;
	std::string name_;
	bool isActive_ = true;
	std::string prefabSource_;
	std::vector<SceneObject*> children_;
};
}

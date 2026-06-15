#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace AOENGINE {

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

	ObjectHandle GetHandle() const;
	void SetHandle(const ObjectHandle& handle);

	const std::string& GetName() const;
	void SetName(const std::string& name);

	bool IsActive() const;
	void SetActive(bool active);

	void AddChild(SceneObject* child);
	void DeleteChild(SceneObject* child);
	bool HasChild() const { return !children_.empty(); }
	const std::vector<SceneObject*>& GetChildren() const { return children_; }

private:
	ObjectHandle handle_;
	std::string name_;
	bool isActive_ = true;
	std::vector<SceneObject*> children_;
};
}

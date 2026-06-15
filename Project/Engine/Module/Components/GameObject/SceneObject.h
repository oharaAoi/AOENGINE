#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace AOENGINE {

struct ObjectHandle {
	uint32_t index;
	uint32_t generation;
};

class SceneObject {
public:
	ObjectHandle GetHandle() const;
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
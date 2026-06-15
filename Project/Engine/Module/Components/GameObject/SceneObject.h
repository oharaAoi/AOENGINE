#pragma once
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

private:
	ObjectHandle handle_;
	std::string name_;
	bool isActive_ = true;
};
}
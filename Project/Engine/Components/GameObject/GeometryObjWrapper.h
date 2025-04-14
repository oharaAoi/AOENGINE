#pragma once
#include "Engine/Components/GameObject/GeometryObject.h"

template<typename Shape>
class GeometryObjWrapper {
public:

	GeometryObjWrapper() = default;
	~GeometryObjWrapper() {};

	template<typename... Args>
	void SetGeometry(Args&&... args) {
		shape_.Init(std::forward<Args>(args)...);
	}

private:

	Shape shape_;

};

#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Engine.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/WorldTransform.h"

class ObjectAxis {
public:

	ObjectAxis();
	~ObjectAxis();

	void Finalize();
	void Init();
	void Update(const Matrix4x4& parentWorldMat);
	void Draw() const;

private:

	Model* model_;
	std::vector<std::unique_ptr<Material>> materials;

	std::unique_ptr<WorldTransform> transform_;

};


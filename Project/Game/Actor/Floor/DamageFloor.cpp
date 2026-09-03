#include "DamageFloor.h"

#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/WinApp/WinApp.h"

void DamageFloor::Init(AOENGINE::BaseGameObject* body) {
	Bind(body);
	parameter_.Load();

	if (!body) { return; }
	AOENGINE::BaseCollider* collider = body->GetCollider("DamageFloor");
	if (!collider) {
		collider = body->SetCollider("DamageFloor", ColliderShape::AABB);
	}
	if (auto* box = dynamic_cast<AOENGINE::BoxCollider*>(collider)) {
		box->SetSize(parameter_.hitSize);
	}
	if (collider) { collider->SetIsTrigger(true); }

	if (AOENGINE::WorldTransform* transform = GetTransform()) {
		position_ = transform->GetTranslate();
	}
}

void DamageFloor::Update(const Math::Matrix4x4& viewProjection) {
	if (!IsValid()) { return; }

	const Math::Vector2 screenPosition{
		parameter_.viewportAnchor.x * static_cast<float>(AOENGINE::WinApp::sClientWidth),
		parameter_.viewportAnchor.y * static_cast<float>(AOENGINE::WinApp::sClientHeight)
	};
	position_ = screenAnchor_.Solve(viewProjection, { screenPosition, parameter_.worldZ });

	if (AOENGINE::WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(position_);
	}
}

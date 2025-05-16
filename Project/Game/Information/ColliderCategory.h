#pragma once
#include <list>
#include <string>

namespace ColliderTags {
	namespace Player {
		const std::string own = "player";
	}

	namespace Boss {
		const std::string own = "boss";
		const std::string attack = "bossAttack";
		const std::string missile = "bossMissile";
	}

	namespace Bullet {
		const std::string machinegun = "machinegun";
		const std::string launcher = "launcher";
	}

	namespace Field {
		const std::string ground = "ground";
	}
}

inline std::list<std::string> GetColliderTagsList() {
	std::list<std::string> colliderTags = {
		ColliderTags::Player::own,

		ColliderTags::Boss::own,
		ColliderTags::Boss::attack,
		ColliderTags::Boss::missile,

		ColliderTags::Bullet::machinegun,
		ColliderTags::Bullet::launcher,

		ColliderTags::Field::ground,
	};

	return colliderTags;
}
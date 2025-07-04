#pragma once
#include <list>
#include <string>

namespace ColliderTags {
	namespace None {
		const std::string own = "none";
	}

	namespace Player {
		const std::string own = "player";
		const std::string leg = "playerLeg";
	}

	namespace Boss {
		const std::string own = "boss";
		const std::string attack = "bossAttack";
		const std::string missile = "bossMissile";
		const std::string armor = "bossArmor";
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
		ColliderTags::None::own,

		ColliderTags::Player::own,
		ColliderTags::Player::leg,

		ColliderTags::Boss::own,
		ColliderTags::Boss::attack,
		ColliderTags::Boss::missile,
		ColliderTags::Boss::armor,

		ColliderTags::Bullet::machinegun,
		ColliderTags::Bullet::launcher,

		ColliderTags::Field::ground,
	};

	return colliderTags;
}
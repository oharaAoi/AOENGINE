#include "BaseCollisionCallBacks.h"

void BaseCollisionCallBacks::SetCallBacks() {
	callBacks_.enter = std::bind(&BaseCollisionCallBacks::CollisionEnter, this, __CALLBACK_PLACEHOLDERS_12);
}

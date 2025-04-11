#include "BaseCollisionCallBacks.h"

void BaseCollisionCallBacks::SetCallBacks() {
	callBacks_.enter = std::bind(&BaseCollisionCallBacks::CollisionEnter, this, __CALLBACK_PLACEHOLDERS_12);
	callBacks_.stay = std::bind(&BaseCollisionCallBacks::CollisionStay, this, __CALLBACK_PLACEHOLDERS_12);
	callBacks_.exit = std::bind(&BaseCollisionCallBacks::CollisionExit, this, __CALLBACK_PLACEHOLDERS_12);
}

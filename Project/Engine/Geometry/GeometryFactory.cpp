#include "GeometryFactory.h"
#include "Engine/Engine.h"

GeometryFactory& GeometryFactory::GetInstance() {
	static GeometryFactory instance;
	return instance;
}

void GeometryFactory::Init() {
	pDevice_ = Engine::GetDevice();
}
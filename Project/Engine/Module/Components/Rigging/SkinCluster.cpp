#include "SkinCluster.h"

using namespace AOENGINE;

SkinCluster::SkinCluster() {
}

SkinCluster::~SkinCluster() {
}

void SkinCluster::Init() {
}

void SkinCluster::AddData(std::map<std::string, JointWeightData> mapData) {
	skinClusterData_ = (std::move(mapData));
}

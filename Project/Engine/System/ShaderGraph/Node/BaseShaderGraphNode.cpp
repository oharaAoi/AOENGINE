#include "BaseShaderGraphNode.h"

BaseShaderGraphNode::~BaseShaderGraphNode() {
}

void BaseShaderGraphNode::BaseInfoFromJson(const nlohmann::json& _json) {
    this->setUID(_json.at("id").get<ImFlow::NodeUID>());
    this->setTitle(_json.at("name").get<std::string>());
    this->setPos(ImVec2(_json.at("pos")[0], _json.at("pos")[1]));
}

void BaseShaderGraphNode::BaseInfoToJson(nlohmann::json& _json) {
    _json["id"] = this->getUID();
    _json["name"] = this->getName();
    _json["pos"] = { this->getPos().x, this->getPos().y };
}

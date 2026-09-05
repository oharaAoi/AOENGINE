#include "ParticleEffect.h"

#include <algorithm>
#include <stdexcept>
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"

using namespace AOENGINE;

namespace {

Math::Vector3 ReadVector3(const json& value, const Math::Vector3& fallback) {
	if (!value.is_array() || value.size() != 3) { return fallback; }
	return { value[0].get<float>(), value[1].get<float>(), value[2].get<float>() };
}

json WriteVector3(const Math::Vector3& value) {
	return json::array({ value.x, value.y, value.z });
}

}

json ParticleEffectAsset::ToJson() const {
	json data;
	data["version"] = version;
	data["name"] = name;
	data["nodes"] = json::array();
	for (const ParticleEffectNode& node : nodes) {
		data["nodes"].push_back({
			{ "name", node.name },
			{ "type", node.type == ParticleEffectNodeType::Gpu ? "GPU" : "CPU" },
			{ "asset", node.asset },
			{ "delay", node.delay },
			{ "position", WriteVector3(node.position) },
			{ "rotation", json::array({ node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w }) },
			{ "scale", WriteVector3(node.scale) },
		});
	}
	return json{ { name, std::move(data) } };
}

ParticleEffectAsset ParticleEffectAsset::FromJson(const json& source, const std::string& fallbackName) {
	const json* data = &source;
	std::string rootName = fallbackName;
	if (source.is_object() && source.size() == 1 && !source.contains("nodes")) {
		rootName = source.begin().key();
		data = &source.begin().value();
	}
	if (!data->is_object() || !data->contains("nodes") || !(*data)["nodes"].is_array()) {
		throw std::runtime_error("ParticleEffect: invalid composite effect JSON");
	}

	ParticleEffectAsset result;
	result.version = data->value("version", kCurrentVersion);
	result.name = data->value("name", rootName);
	if (result.name.empty()) { result.name = rootName; }

	for (const json& item : (*data)["nodes"]) {
		if (!item.is_object()) { continue; }
		ParticleEffectNode node;
		node.name = item.value("name", std::string{});
		node.asset = item.value("asset", std::string{});
		node.type = item.value("type", std::string("CPU")) == "GPU"
			? ParticleEffectNodeType::Gpu : ParticleEffectNodeType::Cpu;
		node.delay = (std::max)(item.value("delay", 0.0f), 0.0f);
		if (item.contains("position")) { node.position = ReadVector3(item["position"], CVector3::ZERO); }
		if (item.contains("scale")) { node.scale = ReadVector3(item["scale"], { 1.0f, 1.0f, 1.0f }); }
		if (item.contains("rotation") && item["rotation"].is_array() && item["rotation"].size() == 4) {
			node.rotation = { item["rotation"][0].get<float>(), item["rotation"][1].get<float>(),
				item["rotation"][2].get<float>(), item["rotation"][3].get<float>() };
		}
		if (node.name.empty()) { node.name = node.asset; }
		if (!node.asset.empty()) { result.nodes.push_back(std::move(node)); }
	}
	return result;
}

ParticleEffectInstance::~ParticleEffectInstance() {
	ReleaseEmitters();
}

bool ParticleEffectInstance::Init(const ParticleEffectAsset& asset, const Math::Vector3& position) {
	ReleaseEmitters();
	asset_ = asset;
	position_ = position;
	elapsedTime_ = 0.0f;
	stopped_ = false;
	finished_ = false;

	for (const ParticleEffectNode& definition : asset_.nodes) {
		RuntimeNode runtime;
		runtime.definition = definition;
		if (definition.type == ParticleEffectNodeType::Gpu) {
			auto* emitter = GpuParticleManager::GetInstance()->CreateEmitter(definition.asset);
			emitter->SetIsStop(true);
			runtime.emitter = emitter;
		} else {
			auto* emitter = ParticleManager::GetInstance()->CreateParticle(definition.asset);
			emitter->SetIsStop(true);
			runtime.emitter = emitter;
		}
		nodes_.push_back(std::move(runtime));
	}
	ApplyTransforms();
	for (RuntimeNode& node : nodes_) {
		if (node.definition.delay <= 0.0f) { StartNode(node); }
	}
	return !nodes_.empty();
}

void ParticleEffectInstance::Update(float deltaTime) {
	if (stopped_ || finished_) { return; }
	elapsedTime_ += (std::max)(deltaTime, 0.0f);
	ApplyTransforms();
	for (RuntimeNode& node : nodes_) {
		if (!node.started && elapsedTime_ >= node.definition.delay) { StartNode(node); }
	}

	// Effect自体に寿命は持たせず、全Emitterが射出を終えたら破棄対象にする。
	// delay待ちのNodeは、未開始なので終了とみなさない。
	finished_ = std::all_of(nodes_.begin(), nodes_.end(), [](const RuntimeNode& node) {
		if (!node.started) { return false; }
		return std::visit([](const auto* emitter) {
			return emitter == nullptr || emitter->GetIsStop();
		}, node.emitter);
	});
}

void ParticleEffectInstance::Stop() {
	stopped_ = true;
	for (RuntimeNode& node : nodes_) {
		std::visit([](auto* emitter) { if (emitter) { emitter->SetIsStop(true); } }, node.emitter);
	}
}

void ParticleEffectInstance::Restart() {
	elapsedTime_ = 0.0f;
	stopped_ = false;
	finished_ = false;
	for (RuntimeNode& node : nodes_) {
		node.started = false;
		std::visit([](auto* emitter) { if (emitter) { emitter->SetIsStop(true); } }, node.emitter);
		if (node.definition.delay <= 0.0f) { StartNode(node); }
	}
}

void ParticleEffectInstance::SetPosition(const Math::Vector3& position) {
	position_ = position;
	ApplyTransforms();
}

void ParticleEffectInstance::SetParent(WorldTransform* parent) {
	parent_ = parent;
	ApplyTransforms();
}

void ParticleEffectInstance::StartNode(RuntimeNode& node) {
	node.started = true;
	std::visit([](auto* emitter) {
		if (!emitter) { return; }
		emitter->Reset();
		emitter->SetIsStop(false);
	}, node.emitter);
}

void ParticleEffectInstance::ApplyTransforms() {
	rootMatrix_ = position_.MakeTranslateMat();
	if (parent_) { rootMatrix_ = rootMatrix_ * parent_->GetWorldMatrix(); }
	for (RuntimeNode& node : nodes_) {
		if (auto** cpu = std::get_if<BaseParticles*>(&node.emitter)) {
			(*cpu)->SetParentMatrix(rootMatrix_);
			(*cpu)->SetPos(node.definition.position);
		} else if (auto** gpu = std::get_if<GpuParticleEmitter*>(&node.emitter)) {
			(*gpu)->SetParent(rootMatrix_);
			(*gpu)->SetLocalPos(node.definition.position);
		}
	}
}

void ParticleEffectInstance::ReleaseEmitters() {
	for (RuntimeNode& node : nodes_) {
		if (auto** cpu = std::get_if<BaseParticles*>(&node.emitter)) {
			ParticleManager::GetInstance()->DeleteParticles(*cpu);
		} else if (auto** gpu = std::get_if<GpuParticleEmitter*>(&node.emitter)) {
			GpuParticleManager::GetInstance()->DeleteEmitter(*gpu);
		}
	}
	nodes_.clear();
}

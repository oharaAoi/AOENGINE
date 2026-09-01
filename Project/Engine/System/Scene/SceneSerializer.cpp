#include "SceneSerializer.h"

#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

#include "Engine/Lib/Json/JsonSerializer.h"
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/2d/Text.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/Collider/LineCollider.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/Render/SceneRenderer.h"

using json = nlohmann::json;
using namespace AOENGINE;

namespace {

json Vector2ToJson(const Math::Vector2& value) { return json::array({ value.x, value.y }); }
json Vector3ToJson(const Math::Vector3& value) { return json::array({ value.x, value.y, value.z }); }
json QuaternionToJson(const Math::Quaternion& value) { return json::array({ value.x, value.y, value.z, value.w }); }
json ColorToJson(const Color& value) { return json::array({ value.r, value.g, value.b, value.a }); }

Math::Vector2 JsonToVector2(const json& value) { return { value.at(0).get<float>(), value.at(1).get<float>() }; }
Math::Vector3 JsonToVector3(const json& value) { return { value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>() }; }
Math::Quaternion JsonToQuaternion(const json& value) {
	return { value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>(), value.at(3).get<float>() };
}
Color JsonToColor(const json& value) {
	return { value.at(0).get<float>(), value.at(1).get<float>(), value.at(2).get<float>(), value.at(3).get<float>() };
}

json SerializeMaterial(const BaseMaterial& material, uint32_t slot) {
	const Math::SRT& uv = material.GetUvTransform();
	json data = {
		{ "slot", slot },
		{ "shaderType", static_cast<int>(material.GetShaderType()) },
		{ "albedoTexture", material.GetAlbedoTexture() },
		{ "uvScale", Vector3ToJson(uv.scale) },
		{ "uvRotate", Vector3ToJson(uv.rotate) },
		{ "uvTranslate", Vector3ToJson(uv.translate) },
		{ "discardValue", material.GetDiscardValue() }
	};

	if (const auto* pbr = dynamic_cast<const PBRMaterial*>(&material)) {
		const PBRMaterial::PBRMaterialData& source = pbr->GetMaterialData();
		data["type"] = "PBR";
		data["color"] = ColorToJson(source.color);
		data["enableLighting"] = source.enableLighting;
		data["diffuseColor"] = ColorToJson(source.diffuseColor);
		data["specularColor"] = ColorToJson(source.specularColor);
		data["roughness"] = source.roughness;
		data["metallic"] = source.metallic;
		data["shininess"] = source.shininess;
		data["ambientIntensity"] = source.ambientIntensity;
		data["normalMap"] = pbr->GetNormalMap();
	} else if (const auto* normal = dynamic_cast<const Material*>(&material)) {
		const Material::MaterialData& source = normal->GetMaterialData();
		data["type"] = "Normal";
		data["color"] = ColorToJson(material.GetColor());
		data["enableLighting"] = material.GetIsLighting();
		data["shininess"] = source.shininess;
		data["iblScale"] = source.iblScale;
	}
	return data;
}

MaterialType GetMaterialType(const json& data) {
	return data.value("type", "Normal") == "PBR" ? MaterialType::PBR : MaterialType::Normal;
}

void DeserializeMaterial(BaseMaterial& material, const json& data) {
	material.SetShaderType(static_cast<MaterialShaderType>(data.value("shaderType", 0)));
	material.SetAlbedoTexture(data.value("albedoTexture", material.GetAlbedoTexture()));
	if (data.contains("uvScale")) { material.SetUvScale(JsonToVector3(data.at("uvScale"))); }
	if (data.contains("uvRotate")) { material.SetUvRotate(JsonToVector3(data.at("uvRotate"))); }
	if (data.contains("uvTranslate")) { material.SetUvTranslate(JsonToVector3(data.at("uvTranslate"))); }
	material.SetDiscardValue(data.value("discardValue", material.GetDiscardValue()));

	if (auto* pbr = dynamic_cast<PBRMaterial*>(&material)) {
		const PBRMaterial::PBRMaterialData& current = pbr->GetMaterialData();
		pbr->SetSceneParameters(
			data.contains("color") ? JsonToColor(data.at("color")) : current.color,
			data.value("enableLighting", current.enableLighting),
			data.contains("diffuseColor") ? JsonToColor(data.at("diffuseColor")) : current.diffuseColor,
			data.contains("specularColor") ? JsonToColor(data.at("specularColor")) : current.specularColor,
			data.value("roughness", current.roughness), data.value("metallic", current.metallic),
			data.value("shininess", current.shininess),
			data.value("ambientIntensity", current.ambientIntensity), data.value("normalMap", ""));
	} else if (auto* normal = dynamic_cast<Material*>(&material)) {
		const Material::MaterialData& current = normal->GetMaterialData();
		if (data.contains("color")) { normal->SetColor(JsonToColor(data.at("color"))); }
		normal->SetIsLighting(data.value("enableLighting", normal->GetIsLighting()));
		normal->SetSceneParameters(data.value("shininess", current.shininess),
			data.value("iblScale", current.iblScale));
	}
	material.Update();
}

const char* GetColliderTypeName(const BaseCollider& collider) {
	const auto& shape = collider.GetShape();
	if (std::holds_alternative<Math::Sphere>(shape)) { return "SphereCollider"; }
	if (std::holds_alternative<Math::AABB>(shape)) { return "AABBCollider"; }
	if (std::holds_alternative<Math::OBB>(shape)) { return "OBBCollider"; }
	if (std::holds_alternative<Math::Line>(shape)) { return "LineCollider"; }
	return "UnknownCollider";
}

json SerializeComponents(const BaseGameObject& object) {
	json components = json::array();

	if (const Animator* animator = object.GetAnimator()) {
		const AnimationClip* clip = animator->GetAnimationClip();
		json data = { { "type", "Animator" } };
		if (clip) {
			data["animationName"] = clip->GetAnimationName();
			data["animationTime"] = clip->GetAnimationTime();
			data["speed"] = clip->GetAnimationSpeed();
			data["loop"] = clip->GetIsLoop();
			data["stopped"] = clip->GetIsStop();
		}
		components.push_back(std::move(data));
	}

	if (const Rigidbody* rigidbody = object.GetRigidbody()) {
		components.push_back({
			{ "type", "Rigidbody" },
			{ "velocity", Vector3ToJson(rigidbody->GetVelocity()) },
			{ "moveForce", Vector3ToJson(rigidbody->GetMoveForce()) },
			{ "gravity", rigidbody->GetGravity() },
			{ "gravityAccel", Vector3ToJson(rigidbody->GetGravityAccel()) },
			{ "drag", rigidbody->GetDrag() }
		});
	}

	for (const BaseCollider* collider : object.GetColliders()) {
		if (!collider) { continue; }
		json data = {
			{ "type", GetColliderTypeName(*collider) },
			{ "category", collider->GetCategoryName() },
			{ "localPosition", Vector3ToJson(collider->GetLocalPos()) },
			{ "active", collider->GetIsActive() },
			{ "static", collider->GetIsStatic() },
			{ "trigger", collider->GetIsTrigger() },
			{ "collisionMask", collider->GetCollisionMaskBit() },
			{ "penetrationPrevention", collider->GetPenetrationPrevention() }
		};

		if (const auto* box = dynamic_cast<const BoxCollider*>(collider)) {
			data["size"] = Vector3ToJson(box->GetSize());
		} else if (std::holds_alternative<Math::Sphere>(collider->GetShape())) {
			data["radius"] = collider->GetRadius();
		} else if (const auto* line = dynamic_cast<const LineCollider*>(collider)) {
			data["diff"] = Vector3ToJson(line->GetDiff());
		}
		components.push_back(std::move(data));
	}

	return components;
}

void DeserializeComponents(BaseGameObject& object, const json& components) {
	if (!components.is_array()) { return; }

	for (const json& data : components) {
		const std::string type = data.value("type", "");
		if (type == "Animator") {
			if (!object.GetModel()) { continue; }
			object.SetAnimator();
			Animator* animator = object.GetAnimator();
			AnimationClip* clip = animator ? animator->GetAnimationClip() : nullptr;
			if (!clip) { continue; }
			const std::string animationName = data.value("animationName", "");
			if (!animationName.empty()) { clip->ResetAnimation(animationName); }
			clip->SetAnimationTime(data.value("animationTime", clip->GetAnimationTime()));
			clip->SetAnimationSpeed(data.value("speed", clip->GetAnimationSpeed()));
			clip->SetIsLoop(data.value("loop", clip->GetIsLoop()));
			clip->SetIsStop(data.value("stopped", clip->GetIsStop()));
			continue;
		}

		if (type == "Rigidbody") {
			object.SetPhysics();
			Rigidbody* rigidbody = object.GetRigidbody();
			if (!rigidbody) { continue; }
			if (data.contains("velocity")) { rigidbody->SetVelocity(JsonToVector3(data.at("velocity"))); }
			if (data.contains("moveForce")) { rigidbody->SetMoveForce(JsonToVector3(data.at("moveForce"))); }
			rigidbody->SetGravity(data.value("gravity", rigidbody->GetGravity()));
			if (data.contains("gravityAccel")) { rigidbody->SetGravityAccel(JsonToVector3(data.at("gravityAccel"))); }
			rigidbody->SetDrag(data.value("drag", rigidbody->GetDrag()));
			continue;
		}

		ColliderShape shape;
		if (type == "SphereCollider") { shape = ColliderShape::Sphere; }
		else if (type == "AABBCollider") { shape = ColliderShape::AABB; }
		else if (type == "OBBCollider") { shape = ColliderShape::OBB; }
		else if (type == "LineCollider") { shape = ColliderShape::Line; }
		else { continue; }

		BaseCollider* collider = object.SetCollider(data.value("category", "Default"), shape);
		if (!collider) { continue; }
		if (data.contains("localPosition")) { collider->SetLocalPos(JsonToVector3(data.at("localPosition"))); }
		collider->SetIsActive(data.value("active", collider->GetIsActive()));
		collider->SetIsStatic(data.value("static", collider->GetIsStatic()));
		collider->SetIsTrigger(data.value("trigger", collider->GetIsTrigger()));
		collider->SetCollisionMaskBits(data.value("collisionMask", collider->GetCollisionMaskBit()));
		collider->SetPenetrationPrevention(data.value("penetrationPrevention", collider->GetPenetrationPrevention()));

		if (auto* box = dynamic_cast<BoxCollider*>(collider); box && data.contains("size")) {
			box->SetSize(JsonToVector3(data.at("size")));
		} else if (shape == ColliderShape::Sphere && data.contains("radius")) {
			collider->SetRadius(data.at("radius").get<float>());
		} else if (auto* line = dynamic_cast<LineCollider*>(collider); line && data.contains("diff")) {
			line->SetDiff(JsonToVector3(data.at("diff")));
		}
	}
}

json SerializeSprite(const Sprite& sprite) {
	const Math::SRT& transform = sprite.GetTransform()->GetTransform();
	return {
		{ "texture", sprite.GetTextureName() },
		{ "translate", Vector3ToJson(transform.translate) },
		{ "rotate", Vector3ToJson(transform.rotate) },
		{ "scale", Vector3ToJson(transform.scale) },
		{ "color", ColorToJson(sprite.GetColor()) },
		{ "drawRange", Vector2ToJson(sprite.GetDrawRange()) },
		{ "leftTop", Vector2ToJson(sprite.GetLeftTop()) },
		{ "anchor", Vector2ToJson(sprite.GetAnchorPoint()) },
		{ "flipX", sprite.GetIsFlipX() },
		{ "flipY", sprite.GetIsFlipY() },
		{ "renderQueue", sprite.GetRenderQueue() }
	};
}

json SerializeObject(const SceneObject& object) {
	json data = json::object();
	if (const BaseGameObject* gameObject = dynamic_cast<const BaseGameObject*>(&object)) {
		if (gameObject->GetModel()) { data["model"] = gameObject->GetModel()->GetName(); }
		if (gameObject->GetTransform()) {
			const Math::QuaternionSRT& srt = gameObject->GetTransform()->GetSRT();
			data["translate"] = Vector3ToJson(srt.translate);
			data["rotate"] = QuaternionToJson(srt.rotate);
			data["scale"] = Vector3ToJson(srt.scale);
		}
		data["enableShadow"] = gameObject->GetEnableShadow();
		data["animator"] = gameObject->GetAnimator() != nullptr;
		data["components"] = SerializeComponents(*gameObject);
		data["materials"] = json::array();
		const auto& materialSlots = gameObject->GetMaterialSlots();
		for (uint32_t slot = 0; slot < materialSlots.size(); ++slot) {
			if (materialSlots[slot]) {
				data["materials"].push_back(SerializeMaterial(*materialSlots[slot], slot));
			}
		}
	} else if (const Text* text = dynamic_cast<const Text*>(&object)) {
		data = SerializeSprite(*text);
		data["text"] = text->GetText();
		data["fontPath"] = text->GetFontPath();
		data["fontSize"] = text->GetFontSize();
		data["textColor"] = ColorToJson(text->GetTextColor());
		data["textAnchor"] = Vector2ToJson(text->GetTextAnchorPoint());
	} else if (const Sprite* sprite = dynamic_cast<const Sprite*>(&object)) {
		data = SerializeSprite(*sprite);
	}
	return data;
}

void DeserializeSprite(Sprite& sprite, const json& data) {
	Math::SRT transform{};
	transform.translate = JsonToVector3(data.at("translate"));
	transform.rotate = JsonToVector3(data.at("rotate"));
	transform.scale = JsonToVector3(data.at("scale"));
	sprite.GetTransform()->SetSRT(transform);
	sprite.SetColor(JsonToColor(data.at("color")));
	sprite.SetDrawRange(JsonToVector2(data.at("drawRange")));
	sprite.SetLeftTop(JsonToVector2(data.at("leftTop")));
	sprite.SetAnchorPoint(JsonToVector2(data.at("anchor")));
	sprite.SetIsFlipX(data.value("flipX", false));
	sprite.SetIsFlipY(data.value("flipY", false));
	sprite.SetRenderQueue(data.value("renderQueue", 0));
}

SceneObject* CreateObject(const json& objectJson, SceneRenderer& renderer, Canvas2d& canvas) {
	const std::string type = objectJson.at("type").get<std::string>();
	const std::string name = objectJson.at("name").get<std::string>();
	const json& data = objectJson.at("data");

	if (type == "BaseGameObject") {
		const json* firstMaterial = nullptr;
		if (data.contains("materials") && data.at("materials").is_array() && !data.at("materials").empty()) {
			firstMaterial = &data.at("materials").front();
		}
		const MaterialType initialMaterialType = firstMaterial ? GetMaterialType(*firstMaterial) : MaterialType::Normal;
		const char* renderingName = initialMaterialType == MaterialType::PBR
			? "Object_PBR.json" : "Object_Normal.json";
		BaseGameObject* object = renderer.AddObject<BaseGameObject>(name, renderingName);
		if (!object) { return nullptr; }
		const std::string modelName = data.value("model", "");
		if (!modelName.empty()) {
			object->SetObject(modelName, initialMaterialType);
			if (data.contains("materials") && data.at("materials").is_array()) {
				for (const json& materialData : data.at("materials")) {
					const uint32_t slot = materialData.value("slot", 0u);
					const MaterialType slotType = GetMaterialType(materialData);
					BaseMaterial* material = object->GetMaterial(slot);
					const bool typeMatches = slotType == MaterialType::PBR
						? dynamic_cast<PBRMaterial*>(material) != nullptr
						: dynamic_cast<Material*>(material) != nullptr;
					if (!typeMatches && object->SetMaterialSlotType(slot, slotType)) {
						material = object->GetMaterial(slot);
					}
					if (material) { DeserializeMaterial(*material, materialData); }
				}
			}
		}
		if (data.contains("translate") && object->GetTransform()) {
			Math::QuaternionSRT srt{};
			srt.translate = JsonToVector3(data.at("translate"));
			srt.rotate = JsonToQuaternion(data.at("rotate"));
			srt.scale = JsonToVector3(data.at("scale"));
			object->GetTransform()->SetSRT(srt);
		}
		object->SetEnableShadow(data.value("enableShadow", true));
		if (data.contains("components")) {
			DeserializeComponents(*object, data.at("components"));
		} else if (data.value("animator", false) && object->GetModel()) {
			// version 1の旧シーンとの互換性を維持する。
			object->SetAnimator();
		}
		return object;
	}
	if (type == "Text") {
		Text* text = canvas.AddText(name, data.value("text", "New Text"), data.value("renderQueue", 0));
		if (!text) { return nullptr; }
		DeserializeSprite(*text, data);
		text->SetFontPath(data.value("fontPath", text->GetFontPath()));
		text->SetFontSize(data.value("fontSize", text->GetFontSize()));
		if (data.contains("textColor")) { text->SetTextColor(JsonToColor(data.at("textColor"))); }
		if (data.contains("textAnchor")) { text->SetTextAnchorPoint(JsonToVector2(data.at("textAnchor"))); }
		return text;
	}
	if (type == "Sprite") {
		Sprite* sprite = canvas.AddSprite(data.value("texture", "white.png"), name, data.value("renderQueue", 0));
		if (!sprite) { return nullptr; }
		DeserializeSprite(*sprite, data);
		return sprite;
	}
	return nullptr;
}

}

bool SceneSerializer::Save(const std::string& folderPath, const std::string& fileName,
	const SceneRenderer& renderer) {
	json root = {
		{ "format", "AOENGINE_SCENE" },
		{ "version", 1 },
		{ "sceneName", fileName },
		{ "objects", json::array() }
	};

	const SceneWorld& world = renderer.GetSceneWorld();
	for (const ObjectHandle& handle : world.GetObjectHandles()) {
		const SceneObject* object = world.FindObject(handle);
		if (!object || object->GetScenePersistence() != ScenePersistence::SceneData) { continue; }

		json objectJson = {
			{ "id", object->GetSceneId() },
			{ "type", object->GetSceneTypeName() },
			{ "name", object->GetName() },
			{ "active", object->IsActive() },
			{ "data", SerializeObject(*object) }
		};
		const ObjectHandle parentHandle = world.GetParentHandle(handle);
		const SceneObject* parent = world.FindObject(parentHandle);
		objectJson["parentId"] = parent && parent->GetScenePersistence() == ScenePersistence::SceneData
			? json(parent->GetSceneId()) : json(nullptr);
		root["objects"].push_back(std::move(objectJson));
	}

	return JsonSerializer::Save(folderPath, fileName, root);
}

bool SceneSerializer::Load(const std::string& folderPath, const std::string& fileName,
	SceneRenderer& renderer, Canvas2d& canvas) {
	const json root = JsonSerializer::Load(folderPath, fileName);
	if (!root.is_object() || root.value("format", "") != "AOENGINE_SCENE" || root.value("version", 0) != 1) {
		return false;
	}

	renderer.GetSceneWorld().ClearSceneObjects();
	canvas.Update();

	std::unordered_map<uint64_t, ObjectHandle> loadedObjects;
	for (const json& objectJson : root.at("objects")) {
		SceneObject* object = CreateObject(objectJson, renderer, canvas);
		if (!object) { continue; }
		const uint64_t id = objectJson.at("id").get<uint64_t>();
		object->SetSceneId(id);
		object->SetActive(objectJson.value("active", true));
		loadedObjects[id] = object->GetHandle();
	}

	for (const json& objectJson : root.at("objects")) {
		if (objectJson.at("parentId").is_null()) { continue; }
		const uint64_t id = objectJson.at("id").get<uint64_t>();
		const uint64_t parentId = objectJson.at("parentId").get<uint64_t>();
		if (loadedObjects.contains(id) && loadedObjects.contains(parentId)) {
			renderer.SetParent(loadedObjects.at(id), loadedObjects.at(parentId));
		}
	}
	canvas.AttachItemsToCanvas();
	return true;
}

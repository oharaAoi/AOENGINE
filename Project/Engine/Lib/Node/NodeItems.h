#pragma once
#include <string>
#include <vector>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/Math/Vector2.h"

enum class PinType {
	Flow,
	Bool,
	Int,
	Float,
	Vector2,
	Vector3,
	Color,
	String,
	Object,
	Texture,
	Function,
	Delegate,
};

enum class PinKind{
    Output,
    Input
};

struct Pin {
	ax::NodeEditor::PinId id;
	ax::NodeEditor::PinKind kind;
};

struct Node {
	ax::NodeEditor::NodeId id;
	Pin inputId;
	Pin outputId;
	std::string name;
};

struct Link {
	ax::NodeEditor::LinkId id;
	ax::NodeEditor::PinId  from;
	ax::NodeEditor::PinId  to;
};

struct NodeItems {
	std::string nodeName;
	int nodeType;
	Vector2 nodePos;
	std::vector<std::string> childName;
};

namespace Custom {
	struct Pin {
		int32_t id;
		PinKind kind;
		PinType type;
		std::string name;

		Pin(int32_t _id, PinKind _kind, PinType _type, const char* _name) :
			id(_id), kind(_kind), type(_type), name(_name) {
		}
	};

	struct Node {
		int32_t id;
		std::vector<Pin> inputs;
		std::vector<Pin> outputs;
		std::string name;
		ImVec2 position;
	};

	struct Link {
		int32_t id;
		int32_t from;
		int32_t to;
	};
}

struct NodeUVTransform {
	Vector2 scale = CVector2::UNIT;
	Vector2 translate = CVector2::ZERO;
	float rotate = 0;
};
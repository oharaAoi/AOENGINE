#pragma once

namespace AOENGINE {

class IEditorWindow {
public:

	IEditorWindow() = default;
	virtual ~IEditorWindow() = default;

public:

	virtual void InspectorWindow() = 0;

	virtual void HierarchyWindow() = 0;

	virtual void ExecutionWindow() = 0;

};

}
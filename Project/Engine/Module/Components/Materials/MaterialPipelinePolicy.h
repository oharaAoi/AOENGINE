#pragma once
#include <string>
#include <vector>

namespace AOENGINE {

// Only shader interfaces implemented by the model Material binders are selectable.
// Pipeline JSON variants may change blend, culling and depth with these interfaces.
inline bool IsMaterialPipelineCompatible(bool pbr, const std::string& vs, const std::string& ps,
	const std::vector<std::string>& renderTargets, const std::string& topology) {
	if (renderTargets != std::vector<std::string>{"16_RGB", "16_RGB"}) { return false; }
	if (!topology.empty() && topology != "Triangle") { return false; }
	if (pbr) {
		return vs == "/Engine/HLSL/PBR.VS.hlsl" && ps == "/Engine/HLSL/PBR.PS.hlsl";
	}
	return vs == "/Engine/HLSL/Object3d.VS.hlsl" &&
		(ps == "/Engine/HLSL/Object3d.PS.hlsl" || ps == "/Engine/HLSL/Object3dEnviroment.PS.hlsl");
}

}

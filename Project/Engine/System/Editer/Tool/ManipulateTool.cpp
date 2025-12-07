#include "ManipulateTool.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Lib/Math/Vector4.h"

using namespace AOENGINE;

UseManipulate ManipulateTool::type_ = UseManipulate::Translate;
bool ManipulateTool::isActive_ = false;

void ManipulateTool::SelectUseManipulate() {
	ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("##ManipulateItem", nullptr, flags)) {
		TextureManager* tex = AOENGINE::TextureManager::GetInstance();
		ImVec2 iconSize(16, 16);
		D3D12_GPU_DESCRIPTOR_HANDLE scaleHandle = tex->GetDxHeapHandles("scale.png").handleGPU;
		D3D12_GPU_DESCRIPTOR_HANDLE rotateHandle = tex->GetDxHeapHandles("rotate.png").handleGPU;
		D3D12_GPU_DESCRIPTOR_HANDLE translateHandle = tex->GetDxHeapHandles("translate.png").handleGPU;
	
		ImTextureID scaleTex = reinterpret_cast<ImTextureID>(scaleHandle.ptr);
		ImTextureID rotateTex = reinterpret_cast<ImTextureID>(rotateHandle.ptr);
		ImTextureID translateTex = reinterpret_cast<ImTextureID>(translateHandle.ptr);
		
		ImGui::Checkbox("##activeManipulate", &isActive_);
		
		Button(scaleTex, "scale", UseManipulate::Scale);
		Button(rotateTex, "rotate", UseManipulate::Rotate);
		Button(translateTex, "translate", UseManipulate::Translate);
	}
	ImGui::End();
}

void ManipulateTool::Button(const ImTextureID& texId, const std::string& label, UseManipulate checkType) {
	Math::Vector4 color = Math::Vector4(25, 25, 112, 255.0f);
	ImVec2 iconSize(16, 16);
	std::string id = "##" + label;
	bool flag = false;

	if (type_ == checkType) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f, color.w / 255.0f)); // 背景色
	}
	if (ImGui::ImageButton(id.c_str(), texId, iconSize)) {
		type_ = checkType;
		flag = true;
	}
	if (!flag) {
		if (type_ == checkType) {
			ImGui::PopStyleColor(1);
		}
	}
}

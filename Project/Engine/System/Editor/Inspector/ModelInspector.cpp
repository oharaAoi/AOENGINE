#include "ModelInspector.h"
#include "ModelInspector.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/System/Asset/AssetHandle.h"

using namespace AOENGINE;

void AOENGINE::ModelInspector::Draw(BaseGameObject& object) {
    if (!ImGui::CollapsingHeader("Model")) {
        return;
    }

    Model* currentModel = object.GetModel();

    if (currentModel) {
        ImGui::Text("Current: %s", currentModel->GetName().c_str());
    } else {
        ImGui::Text("Current: null");
    }

    // AssetWindow から Model をドラッグ&ドロップ
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_HANDLE")) {
            const AssetHandle& handle = *static_cast<const AssetHandle*>(payload->Data);

            if (handle.type == AssetType::Model) {
                Model* model = ModelManager::GetInstance()->SearchModel(handle.id);
                if (model) {
                    object.SetObject(model->GetName());
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    // 読み込み済みモデル一覧から選択
    auto& modelNames = ModelManager::GetInstance()->GetModelNames();

    const char* previewName = currentModel ? currentModel->GetName().c_str() : "None";

    if (ImGui::BeginCombo("Model", previewName)) {
        for (const std::string& modelName : modelNames) {
            bool selected = currentModel && currentModel->GetName() == modelName;

            if (ImGui::Selectable(modelName.c_str(), selected)) {
                object.SetObject(modelName);
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

#include "GameObjectWindow.h"
#include <sstream>
#include "Engine/Utilities/ImGuiHelperFunc.h"

GameObjectWindow::GameObjectWindow() {}
GameObjectWindow::~GameObjectWindow() {}

void GameObjectWindow::Init() {
	attributeArray_.clear();
	selectAttribute_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectWindow::AddAttributeGui(AttributeGui* attribute, const std::string& label) {
	std::string uniqueLabel = label;

	std::string newName = MakeUniqueName(uniqueLabel);
	attribute->SetName(newName);

	// ユニークなラベルでリストに追加
	attributeArray_.emplace_back(attribute);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集画面を表示する
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#include "Engine/System/Manager/ImGuiManager.h"
void GameObjectWindow::Edit() {
	if (attributeArray_.empty()) { return; }

	// -------------------------------------------------
	// ↓ Itemの選択
	// -------------------------------------------------
	ImGui::Begin("Scene Object");
	static std::string openNode = "";  // 現在開いているTreeNodeの名前
	static bool firstOpenRoot = true;
	for (auto it : attributeArray_) {
		AttributeGui* ptr = it;
		std::string label = ptr->GetName();
		std::string id = "##" + std::to_string(reinterpret_cast<uintptr_t>(ptr));
		label += id;
		// 子供を所有している場合
		if (ptr->HasChild()) {
			bool isOpen = (label == openNode);  // 現在開いているノードか確認
			if (isOpen) {
				ImGui::SetNextItemOpen(true);  // 強制的に開いた状態にする
			}
			if (ImGui::TreeNode(label.c_str())) {
				if (ImGui::IsItemClicked()) {
					firstOpenRoot = true;
				}
				if (firstOpenRoot) {
					firstOpenRoot = false;
					selectAttribute_ = ptr;
					openNode = "";  // 他のノードを閉じる
				}

				for (auto child : ptr->GetChildren()) {
					if (selectAttribute_ != nullptr) {
						if (ImGui::Selectable(child->GetName().c_str(), selectAttribute_ == child)) {
							// 新しく選択されたら開いているノードを変更
							if (selectAttribute_ != child) {
								openNode = label;  // 現在の親ノードを記録
							}
							selectAttribute_ = child;
						}
					}
				}
				ImGui::TreePop();
			} else {
				// 閉じたら記録をリセット
				if (openNode == label) {
					openNode = "";
				}
			}
		} else {
			// 子供を持たないノードの場合
			if (ImGui::Selectable(label.c_str(), selectAttribute_ == ptr)) {
				selectAttribute_ = it;
				openNode = "";  // 他のノードを閉じる
			}
		}
	}

	ImGui::End();

	// -------------------------------------------------
	// ↓ ItemごとのImGuiを編集する
	// -------------------------------------------------
	ImGui::Begin("Object Setting");
	if (selectAttribute_ != nullptr) {
		bool isActive = selectAttribute_->GetIsActive();
		ImGui::Checkbox(" ", &isActive);
		ImGui::SameLine();
		std::string selectName = selectAttribute_->GetName();
		if (InputTextWithString("Name :", "##selectName", selectName)) {
			selectAttribute_->SetName(MakeUniqueName(selectName));
		}

		ImGui::Separator();
		selectAttribute_->Debug_Gui();
		selectAttribute_->SetIsActive(isActive);
	}
	ImGui::End();
}
std::string GameObjectWindow::MakeUniqueName(const std::string& baseName) {
	int count = 0;
	std::string newName = baseName;

	auto isDuplicate = [&](const std::string& name) {
		for (auto* attr : attributeArray_) {
			if (attr->GetName() == name) return true;
		}
		return false;
		};

	while (isDuplicate(newName)) {
		++count;
		newName = baseName + "(" + std::to_string(count) + ")";
	}

	return newName;
}
#endif // _DEBUG

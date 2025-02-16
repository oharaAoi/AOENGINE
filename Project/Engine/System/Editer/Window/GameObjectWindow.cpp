#include "GameObjectWindow.h"
#include <sstream>

GameObjectWindow::GameObjectWindow() {}
GameObjectWindow::~GameObjectWindow() {}

void GameObjectWindow::Init() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectWindow::AddAttributeGui(AttributeGui* attribute, const std::string& label) {
	std::string uniqueLabel = label;
	int counter = 1;

	// ラベルがすでにリスト内に存在するかチェック
	auto isLabelDuplicate = [&](const std::string& labelToCheck) {
		for (const auto& pair : attributeArray_) {
			if (pair.first == labelToCheck) {
				return true;
			}
		}
		return false;
		};

	// 重複があれば、末尾に番号を追加してユニークにする
	while (isLabelDuplicate(uniqueLabel)) {
		std::ostringstream oss;
		oss << label << counter++;
		uniqueLabel = oss.str();
	}

	// ユニークなラベルでリストに追加
	attributeArray_.emplace_back(uniqueLabel, attribute);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集画面を表示する
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#include "Engine/System/Manager/ImGuiManager.h"
void GameObjectWindow::Edit() {
	// -------------------------------------------------
	// ↓ Itemの選択
	// -------------------------------------------------
	ImGui::Begin("Scene Object");
	static AttributeGui* selectAttribute = nullptr;  // 現在選択されているノード
	static std::string openNode = "";  // 現在開いているTreeNodeの名前
	for (auto it : attributeArray_) {
		std::string label = it.first;
		const AttributeGui* ptr = it.second;
		// 子供を所有している場合
		if (ptr->HasChild()) {
			bool isOpen = (label == openNode);  // 現在開いているノードか確認
			if (isOpen) {
				ImGui::SetNextItemOpen(true);  // 強制的に開いた状態にする
			}

			if (ImGui::TreeNode(label.c_str())) {
				for (auto child : ptr->GetChildren()) {
					if (ImGui::Selectable(child->GetName().c_str(), selectAttribute == child)) {
						// 新しく選択されたら開いているノードを変更
						if (selectAttribute != child) {
							openNode = label;  // 現在の親ノードを記録
						}
						selectAttribute = child;
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
			if (ImGui::Selectable(label.c_str(), selectAttribute == ptr)) {
				selectAttribute = it.second;
				openNode = "";  // 他のノードを閉じる
			}
		}
	}

	ImGui::End();

	// -------------------------------------------------
	// ↓ ItemごとのImGuiを編集する
	// -------------------------------------------------
	ImGui::Begin("Object Setting");
	if (selectAttribute != nullptr) {
		selectAttribute->Debug_Gui();
	}
	ImGui::End();
}
#endif // _DEBUG

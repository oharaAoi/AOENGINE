#include "SceneWorld.h"

#include <algorithm>

using namespace AOENGINE;

void SceneWorld::Clear() {
	reusableObjectIndices_.clear();
	rootObjectHandles_.clear();

	for (uint32_t index = 0; index < objectSlots_.size(); ++index) {
		ObjectSlot& slot = objectSlots_[index];
		if (slot.object) {
			// SceneWorldから外れるため、オブジェクト自身が保持するハンドルも無効化する。
			slot.object->SetHandle(ObjectHandle{});
		}

		slot.ownedObject.reset();
		slot.object = nullptr;
		slot.isAlive = false;

		// generationを進めて、外部が保持している既存ハンドルを失効させる。
		slot.generation = NextGeneration(slot.generation);
		reusableObjectIndices_.push_back(index);
	}
}

ObjectHandle SceneWorld::AddObject(std::unique_ptr<SceneObject> object, const std::string& name) {
	if (!object) {
		return {};
	}

	// スロットを確保してから、オブジェクト側にも同じハンドルを持たせる。
	ObjectHandle handle = AllocateHandle();
	object->SetHandle(handle);
	if (!name.empty()) {
		object->SetName(name);
	}

	ObjectSlot& slot = objectSlots_[handle.index];
	slot.ownedObject = std::move(object);
	slot.object = slot.ownedObject.get();
	slot.isAlive = true;

	// 親が設定されるまではHierarchy上のルートとして扱う。
	AddRootObject(handle);
	return handle;
}

ObjectHandle SceneWorld::AddExternalObject(SceneObject& object, const std::string& name) {
	const ObjectHandle currentHandle = object.GetHandle();
	if (IsValid(currentHandle) && FindObject(currentHandle) == &object) {
		if (!name.empty()) {
			object.SetName(name);
		}
		return currentHandle;
	}

	ObjectHandle handle = AllocateHandle();
	object.SetHandle(handle);
	if (!name.empty()) {
		object.SetName(name);
	}

	ObjectSlot& slot = objectSlots_[handle.index];
	slot.ownedObject.reset();
	slot.object = &object;
	slot.isAlive = true;

	AddRootObject(handle);
	return handle;
}

void SceneWorld::DestroyObject(ObjectHandle handle) {
	SceneObject* object = FindObject(handle);
	if (!object) {
		return;
	}

	std::vector<ObjectHandle> childHandles;
	// DestroyObject中に子リストが更新されるため、先に破棄対象のハンドルを退避する。
	for (SceneObject* child : object->GetChildren()) {
		if (child && child->GetHandle().IsValid()) {
			childHandles.push_back(child->GetHandle());
		}
	}

	// 子階層を先に破棄してから、自身を親やルート一覧から取り除く。
	for (const ObjectHandle& childHandle : childHandles) {
		DestroyObject(childHandle);
	}

	DetachFromParents(object);
	ReleaseHandle(handle);
}

bool SceneWorld::IsValid(ObjectHandle handle) const {
	if (!handle.IsValid() || handle.index >= objectSlots_.size()) {
		return false;
	}

	const ObjectSlot& slot = objectSlots_[handle.index];
	// indexが再利用されていても、generationが違う古いハンドルは無効と判定する。
	return slot.isAlive && slot.generation == handle.generation && slot.object != nullptr;
}

SceneObject* SceneWorld::FindObject(ObjectHandle handle) {
	if (!IsValid(handle)) {
		return nullptr;
	}

	return objectSlots_[handle.index].object;
}

const SceneObject* SceneWorld::FindObject(ObjectHandle handle) const {
	if (!IsValid(handle)) {
		return nullptr;
	}

	return objectSlots_[handle.index].object;
}

std::vector<ObjectHandle> SceneWorld::GetObjectHandles() const {
	std::vector<ObjectHandle> handles;
	handles.reserve(objectSlots_.size());

	for (uint32_t index = 0; index < objectSlots_.size(); ++index) {
		const ObjectSlot& slot = objectSlots_[index];
		if (slot.isAlive && slot.object) {
			handles.push_back(ObjectHandle{ index, slot.generation });
		}
	}

	return handles;
}

std::vector<ObjectHandle> SceneWorld::GetRootObjectHandles() const {
	std::vector<ObjectHandle> handles;
	handles.reserve(rootObjectHandles_.size());

	for (const ObjectHandle& handle : rootObjectHandles_) {
		if (IsValid(handle)) {
			handles.push_back(handle);
		}
	}

	return handles;
}

ObjectHandle SceneWorld::GetParentHandle(ObjectHandle child) const {
	const SceneObject* childObject = FindObject(child);
	if (!childObject) {
		return {};
	}

	// SceneObjectは親ポインタを持たないため、全オブジェクトの子リストから親を探す。
	for (uint32_t index = 0; index < objectSlots_.size(); ++index) {
		const ObjectSlot& slot = objectSlots_[index];
		if (!slot.isAlive || !slot.object) {
			continue;
		}

		const std::vector<SceneObject*>& children = slot.object->GetChildren();
		if (std::find(children.begin(), children.end(), childObject) != children.end()) {
			return ObjectHandle{ index, slot.generation };
		}
	}

	return {};
}

bool SceneWorld::SetParent(ObjectHandle child, ObjectHandle parent) {
	// 無効なparent指定は、childをルートへ戻す操作として扱う。
	if (!parent.IsValid()) {
		return MoveToRoot(child);
	}

	SceneObject* childObject = FindObject(child);
	SceneObject* parentObject = FindObject(parent);
	if (!childObject || !parentObject || child == parent) {
		return false;
	}

	// 子孫を親にすると循環参照になるため禁止する。
	if (ContainsObjectRecursive(childObject, parentObject)) {
		return false;
	}

	// 既存の親またはルート登録から外してから、新しい親の子として接続する。
	DetachFromParents(childObject);
	RemoveRootObject(child);
	parentObject->AddChild(childObject);
	return true;
}

bool SceneWorld::AddChild(ObjectHandle parent, ObjectHandle child) {
	return SetParent(child, parent);
}

bool SceneWorld::RemoveChild(ObjectHandle parent, ObjectHandle child) {
	SceneObject* parentObject = FindObject(parent);
	SceneObject* childObject = FindObject(child);
	if (!parentObject || !childObject) {
		return false;
	}

	const std::vector<SceneObject*>& children = parentObject->GetChildren();
	if (std::find(children.begin(), children.end(), childObject) == children.end()) {
		return false;
	}

	// 親子関係を外したオブジェクトはHierarchy上のルートに戻す。
	parentObject->DeleteChild(childObject);
	return AddRootObject(child);
}

bool SceneWorld::MoveToRoot(ObjectHandle handle) {
	SceneObject* object = FindObject(handle);
	if (!object) {
		return false;
	}

	// 現在の親から外したうえでルート一覧に登録する。
	DetachFromParents(object);
	return AddRootObject(handle);
}

void SceneWorld::Update() {
	std::vector<ObjectHandle> destroyHandles;

	// 更新中にスロット配列を変更しないよう、破棄要求は一度集めて後で処理する。
	for (const ObjectHandle& handle : GetObjectHandles()) {
		SceneObject* object = FindObject(handle);
		if (!object) {
			continue;
		}

		ISceneObject* sceneObject = dynamic_cast<ISceneObject*>(object);
		if (!sceneObject) {
			continue;
		}

		if (object->IsActive()) {
			sceneObject->Update();
		}

		if (sceneObject->GetIsDestroy()) {
			destroyHandles.push_back(handle);
		}
	}

	for (const ObjectHandle& handle : destroyHandles) {
		DestroyObject(handle);
	}
}

void SceneWorld::PostUpdate() {
	std::vector<ObjectHandle> destroyHandles;

	// PostUpdateでもUpdateと同様に、走査完了後に破棄する。
	for (const ObjectHandle& handle : GetObjectHandles()) {
		SceneObject* object = FindObject(handle);
		if (!object) {
			continue;
		}

		ISceneObject* sceneObject = dynamic_cast<ISceneObject*>(object);
		if (!sceneObject) {
			continue;
		}

		if (object->IsActive()) {
			sceneObject->PostUpdate();
		}

		if (sceneObject->GetIsDestroy()) {
			destroyHandles.push_back(handle);
		}
	}

	for (const ObjectHandle& handle : destroyHandles) {
		DestroyObject(handle);
	}
}

bool SceneWorld::IsRootObject(ObjectHandle handle) const {
	if (!IsValid(handle)) {
		return false;
	}

	return std::find(rootObjectHandles_.begin(), rootObjectHandles_.end(), handle) != rootObjectHandles_.end();
}

ObjectHandle SceneWorld::AllocateHandle() {
	uint32_t index = 0;

	if (!reusableObjectIndices_.empty()) {
		// 破棄済みスロットを優先して再利用し、配列の肥大化を抑える。
		index = reusableObjectIndices_.back();
		reusableObjectIndices_.pop_back();
	} else {
		index = static_cast<uint32_t>(objectSlots_.size());
		objectSlots_.push_back(ObjectSlot{});
	}

	ObjectSlot& slot = objectSlots_[index];
	if (slot.generation == 0) {
		// generation 0はObjectHandleの無効値なので、有効な世代へ補正する。
		slot.generation = 1;
	}

	return ObjectHandle{ index, slot.generation };
}

void SceneWorld::ReleaseHandle(ObjectHandle handle) {
	if (!IsValid(handle)) {
		return;
	}

	ObjectSlot& slot = objectSlots_[handle.index];
	if (slot.object) {
		// 解放済みオブジェクトが自身の古いハンドルを参照しないようにする。
		slot.object->SetHandle(ObjectHandle{});
	}

	slot.ownedObject.reset();
	slot.object = nullptr;
	slot.isAlive = false;
	slot.generation = NextGeneration(slot.generation);

	// ルート一覧からも外し、スロットindexは次回以降の追加で再利用する。
	RemoveRootObject(handle);
	reusableObjectIndices_.push_back(handle.index);
}

void SceneWorld::DetachFromParents(SceneObject* object) {
	if (!object) {
		return;
	}

	// SceneObjectは親ポインタを持たないため、全スロットを走査して子参照を消す。
	for (ObjectSlot& slot : objectSlots_) {
		if (slot.isAlive && slot.object) {
			slot.object->DeleteChild(object);
		}
	}
}

void SceneWorld::RemoveRootObject(ObjectHandle handle) {
	rootObjectHandles_.erase(
		std::remove_if(
			rootObjectHandles_.begin(),
			rootObjectHandles_.end(),
			[handle](const ObjectHandle& rootHandle) {
				// generationが変わった古いルート情報もまとめて削除する。
				return rootHandle.index == handle.index;
			}),
		rootObjectHandles_.end());
}

bool SceneWorld::AddRootObject(ObjectHandle handle) {
	if (!IsValid(handle)) {
		return false;
	}

	if (IsRootObject(handle)) {
		return true;
	}

	// 有効かつ未登録のハンドルだけをルート一覧へ追加する。
	rootObjectHandles_.push_back(handle);
	return true;
}

uint32_t SceneWorld::NextGeneration(uint32_t generation) {
	++generation;
	// generation 0は無効なObjectHandleを表すため、桁あふれ時は1へ戻す。
	return generation == 0 ? 1 : generation;
}

bool SceneWorld::ContainsObjectRecursive(const SceneObject* root, const SceneObject* target) {
	if (!root || !target) {
		return false;
	}

	for (const SceneObject* child : root->GetChildren()) {
		// targetを子孫に持つ場合、targetをrootの親にすると循環する。
		if (child == target || ContainsObjectRecursive(child, target)) {
			return true;
		}
	}

	return false;
}

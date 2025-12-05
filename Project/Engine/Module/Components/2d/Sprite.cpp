#include "Sprite.h"
#include "Render.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Module/Geometry/Structs/Vertices.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

using namespace AOENGINE;

Sprite::Sprite() {}
Sprite::~Sprite() {
	vertexData_ = nullptr;
	indexData_ = nullptr;
	materialData_ = nullptr;
	vertexBuffer_.Reset();
	indexBuffer_.Reset();
	materialBuffer_.Reset();
	transform_.reset();
}

void Sprite::Init(const std::string& fileName) {
	AOENGINE::GraphicsContext* ctx = AOENGINE::GraphicsContext::GetInstance();
	ID3D12Device* pDevice = ctx->GetDevice();

	textureSize_ = TextureManager::GetInstance()->GetTextureSize(fileName);
	spriteSize_ = textureSize_;
	textureName_ = fileName;
	drawRange_ = spriteSize_;
	leftTop_ = { 0.0f, 0.0f };
	anchorPoint_ = { 0.5f, 0.5f };

	// -------------------------------------------------
	// ↓ Meshの初期化
	// -------------------------------------------------

	vertexBuffer_ = CreateBufferResource(pDevice, sizeof(TextureMesh) * 4);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(TextureMesh) * 4;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(TextureMesh);
	// Resourceにデータを書き込む 
	vertexData_ = nullptr;
	// アドレスを取得
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	Vector3 pivotOffset = {
		textureSize_.x * anchorPoint_.x,
		textureSize_.y * anchorPoint_.y,
		0.0f
	};

	// スプライトのサイズを基に頂点を設定
	RectangleVertices rect = {
		{-pivotOffset.x, -pivotOffset.y, 0.0f, 1.0f},
		{textureSize_.x - pivotOffset.x, 0.0f - pivotOffset.y, 0.0f, 1.0f},
		{0.0f - pivotOffset.x, textureSize_.y - pivotOffset.y, 0.0f , 1.0f},
		{textureSize_.x - pivotOffset.x, textureSize_.y - pivotOffset.y , 0.0f, 1.0f},
	};

	vertexData_[0].pos = rect.leftBottom;		// 左下
	vertexData_[0].texcoord = { 0.0f, 1.0f };
	vertexData_[1].pos = rect.leftTop;			// 左上
	vertexData_[1].texcoord = { 0.0f, 0.0f };	
	vertexData_[2].pos = rect.rightBottom;		// 右下
	vertexData_[2].texcoord = { 1.0f, 1.0f };
	vertexData_[3].pos = rect.rightTop;			// 右上
	vertexData_[3].texcoord = { 1.0f, 0.0f };

	indexBuffer_ = CreateBufferResource(pDevice, sizeof(uint32_t) * 6);
	indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * 6);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexData_ = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;

	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

	// -------------------------------------------------
	// ↓ Materialの初期化
	// -------------------------------------------------

	materialBuffer_ = CreateBufferResource(pDevice, sizeof(TextureMaterial));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->uvTransform = Matrix4x4::MakeUnit();
	materialData_->uvMinSize = { 0.0f, 0.0f };
	materialData_->uvMaxSize = { 1.0f, 1.0f };
	materialData_->arcType = 0;

	// -------------------------------------------------
	// ↓ Transformの初期化
	// -------------------------------------------------

	transform_ = std::make_unique<ScreenTransform>();
	transform_->Init(pDevice);
	uvTransform_ = { {1.0f,1.0f,1.0f} , {0.0f, 0.0f, 0.0f}, {0, 0, 0} };

	// -------------------------------------------------
	// ↓ 塗りつぶしの初期化
	// -------------------------------------------------

	arcGaugeParamBuffer_ = CreateBufferResource(pDevice, sizeof(ArcGaugeParam));
	arcGaugeParamBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&arcData_));

	arcData_->center = { 0.5f, 0.5f };
	arcData_->fillAmount = 1.0f;
	arcData_->innerRadius = 0.0f;
	arcData_->outerRadius = 0.0f;
	arcData_->startAngle = 0.0f;
	arcData_->arcRange = kPI2;

	// -------------------------------------------------
	// ↓ メンバ変数の初期化
	// -------------------------------------------------

	isEnable_ = true;

	fillMethod_ = FillMethod::Vertical;
	fillStartingPoint_ = FillStartingPoint::Top;

	isEnable_ = true;
	isDestroy_ = false;
	isBackGround_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::Update() {
	if (fillMethod_ == FillMethod::Radial) {
		materialData_->arcType = 1;
	} else {
		materialData_->arcType = 0;
	}

	// 各頂点を更新する
	float left = (0.0f - anchorPoint_.x) * spriteSize_.x;
	float right = (1.0f - anchorPoint_.x) * spriteSize_.x;
	float top = (0.0f - anchorPoint_.y) * spriteSize_.y;
	float bottom = (1.0f - anchorPoint_.y) * spriteSize_.y;

	vertexData_[0].pos = { left, bottom, 0.0f, 1.0f };
	vertexData_[1].pos = { left, top, 0.0f, 1.0f };
	vertexData_[2].pos = { right, bottom, 0.0f, 1.0f };
	vertexData_[3].pos = { right, top, 0.0f, 1.0f };

	// -------------------------------------------------
	// ↓ UVの変更
	// -------------------------------------------------
	materialData_->uvTransform = uvTransform_.MakeAffine();
	materialData_->uvTransform.m[0][0] = drawRange_.x / spriteSize_.x;	// Xスケーリング
	materialData_->uvTransform.m[1][1] = drawRange_.y / spriteSize_.y;	// Yスケーリング
	materialData_->uvTransform.m[3][0] = leftTop_.x / spriteSize_.x;	// Xオフセット
	materialData_->uvTransform.m[3][1] = leftTop_.y / spriteSize_.y;	// Yオフセット
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画前処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::Draw(const Pipeline* pipeline, bool isBackGround) {
	Matrix4x4 projection = AOENGINE::Render::GetViewport2D() * AOENGINE::Render::GetProjection2D();
	if (isBackGround) {
		transform_->SetTranslateZ(AOENGINE::Render::GetFarClip());
	}
	transform_->Update(projection);

	AOENGINE::Render::DrawSprite(this, pipeline);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　実際にDrawCallを呼び出す
//////////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::PostDraw(ID3D12GraphicsCommandList* commandList, const Pipeline* pipeline) const {
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
	UINT index = pipeline->GetRootSignatureIndex("gMaterial");
	commandList->SetGraphicsRootConstantBufferView(index, materialBuffer_->GetGPUVirtualAddress());
	index = pipeline->GetRootSignatureIndex("gArcParam");
	commandList->SetGraphicsRootConstantBufferView(index, arcGaugeParamBuffer_->GetGPUVirtualAddress());
	index = pipeline->GetRootSignatureIndex("gTransformationMatrix");
	transform_->BindCommand(commandList, index);
	index = pipeline->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName_, index);
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Textureを再設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::ReSetTexture(const std::string& fileName) {
	textureName_ = fileName;
	textureSize_ = TextureManager::GetInstance()->GetTextureSize(fileName);
	spriteSize_ = textureSize_;
	drawRange_ = spriteSize_;
	leftTop_ = { 0.0f, 0.0f };

	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;

	vertexData_[0].pos = { left, bottom, 0.0f, 1.0f };
	vertexData_[1].pos = { left, top, 0.0f, 1.0f };
	vertexData_[2].pos = { right, bottom, 0.0f, 1.0f };
	vertexData_[3].pos = { right, top, 0.0f, 1.0f };
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Textureのサイズを再設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::ReSetTextureSize(const Vector2& size) {
	spriteSize_ = size;
}

void Sprite::FillAmount(float amount) {
	switch (fillMethod_) {
	case FillMethod::Vertical:
		if (fillStartingPoint_ == FillStartingPoint::Top) {
			materialData_->uvMaxSize = Vector2(1.0f, amount);
		} else if (fillStartingPoint_ == FillStartingPoint::Bottom) {
			materialData_->uvMinSize = Vector2(0.0f, amount);
		}
		break;
	case FillMethod::Horizontal:
		if (fillStartingPoint_ == FillStartingPoint::Left) {
			materialData_->uvMaxSize = Vector2(amount, 1.0f);
		} else if (fillStartingPoint_ == FillStartingPoint::Right) {
			materialData_->uvMinSize = Vector2(amount, 0.0f);
		}
		break;
	case FillMethod::Radial:
		arcData_->fillAmount = amount;
		break;
	case FillMethod::BothEnds:
		if (fillStartingPoint_ == FillStartingPoint::TopBottom) {
			float halfAmount = amount * 0.5f;
			materialData_->uvMaxSize = Vector2(1.0f - (0.5f - halfAmount), 1.0f);
			materialData_->uvMinSize = Vector2(0.5f - halfAmount, 0.0f);
		} else if (fillStartingPoint_ == FillStartingPoint::LeftRight) {
			float halfAmount = amount * 0.5f;
			materialData_->uvMaxSize = Vector2(1.0f, 1.0f - (0.5f - halfAmount));
			materialData_->uvMinSize = Vector2(0.0f, 0.5f - halfAmount);
		}
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::Debug_Gui() {
	ImGui::Checkbox("isEnable", &isEnable_);
	if (ImGui::Button("ResetSize")) {
		textureSize_ = TextureManager::GetInstance()->GetTextureSize(textureName_);
		spriteSize_ = textureSize_;
		drawRange_ = spriteSize_;
	}

	transform_->Debug_Gui();

	if (ImGui::TreeNode("uv")) {
		ImGui::DragFloat2("uvTranslation", &uvTransform_.translate.x, 0.01f);
		ImGui::DragFloat2("uvScale", &uvTransform_.scale.x, 0.01f);
		ImGui::SliderAngle("uvRotation", &uvTransform_.rotate.z);
		ImGui::TreePop();
	}

	ImGui::DragFloat2("anchorPoint", &anchorPoint_.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("textureSize", &spriteSize_.x, 1.0f);
	ImGui::DragFloat2("drawRange", &drawRange_.x, 1.0f);
	ImGui::DragFloat2("leftTop", &leftTop_.x, 1.0f);
	ImGui::DragFloat2("uvMin", &materialData_->uvMinSize.x, 0.01f);
	ImGui::DragFloat2("uvMax", &materialData_->uvMaxSize.x, 0.01f);

	ImGui::ColorEdit4("color", &materialData_->color.r);
	TextureManager* textureManager = TextureManager::GetInstance();
	textureName_ = textureManager->SelectTexture(textureName_);

	static const char* items[] = { "Vertical", "Horizontal", "Radial", "BothEnds"};
	int current = static_cast<int>(fillMethod_);
	if (ImGui::Combo("Fill Method", &current, items, IM_ARRAYSIZE(items))) {
		fillMethod_ = static_cast<FillMethod>(current);
	}

	switch (fillMethod_) {
	case FillMethod::Vertical:
		{
			static const char* pointItems[] = { "Top", "Bottom" };
			int currentPoint = static_cast<int>(fillStartingPoint_);
			if (ImGui::Combo("Fill Starting Point", &currentPoint, pointItems, IM_ARRAYSIZE(pointItems))) {
				fillStartingPoint_ = static_cast<FillStartingPoint>(currentPoint);
			}
		}
		break;
	case FillMethod::Horizontal:
		{
			static const char* pointItems[] = { "Left", "Right" };
			int currentPoint = static_cast<int>(fillStartingPoint_);
			if (ImGui::Combo("Fill Starting Point", &currentPoint, pointItems, IM_ARRAYSIZE(pointItems))) {
 				fillStartingPoint_ = static_cast<FillStartingPoint>(currentPoint + 2);
			}
		}
		break;
	case FillMethod::Radial:
		{
		ImGui::SliderFloat("FillAmount", &arcData_->fillAmount, 0.0f, 1.0f);
		ImGui::SameLine();
		ImGui::DragFloat(":", &arcData_->fillAmount,0.01f);
		ImGui::DragFloat2("center", &arcData_->center.x, 0.01f);
		ImGui::DragFloat("innerRadius",&arcData_->innerRadius, 0.01f);
		ImGui::DragFloat("outerRadius",&arcData_->outerRadius, 0.01f);

		float startAngle = (arcData_->startAngle) * kToDegree;
		ImGui::DragFloat("startAngle",&startAngle, 1.0f, 0.0f, 360.0f);
		arcData_->startAngle = startAngle * kToRadian;

		float arcRange = (arcData_->arcRange) * kToDegree;
		ImGui::DragFloat("arcRange",&arcRange, 1.0f, 0.0f, 360.0f);
		arcData_->arcRange = arcRange * kToRadian;
		
		bool clockwise = arcData_->clockwise;
		ImGui::Checkbox("clockwise", &clockwise);
		arcData_->clockwise = clockwise;
		break;
		}
	case FillMethod::BothEnds:
	{
		static const char* pointItems[] = { "TopBottom", "LeftRight" };
		int currentPoint = static_cast<int>(fillStartingPoint_) - 3;
		if (ImGui::Combo("Fill Starting Point", &currentPoint, pointItems, IM_ARRAYSIZE(pointItems))) {
			fillStartingPoint_ = static_cast<FillStartingPoint>(currentPoint + 3);
		}
	}
		break;
	default:
		break;
	}

	if (ImGui::CollapsingHeader("Save & Load")) {
		InputTextWithString("GroupName", "##group", saveGroupName_);
		InputTextWithString("KeyName", "##key", saveKeyName_);

		if (ImGui::Button("Save")) {
			Save(saveGroupName_, saveKeyName_);
		}
		if (ImGui::Button("Load")) {
			Load(saveGroupName_, saveKeyName_);
		}
	}
}

void Sprite::ApplyParam() {
	transform_->SetSRT(saveParam_.transform);
	uvTransform_ = saveParam_.uvTransform;
	textureName_ = saveParam_.textureName;
	materialData_->color = saveParam_.color;
	drawRange_ = saveParam_.drawRange;
	leftTop_ = saveParam_.leftTop;
	anchorPoint_ = saveParam_.anchorPoint;
	isFlipX_ = saveParam_.isFlipX;
	isFlipY_ = saveParam_.isFlipY;
	spriteSize_ = saveParam_.spriteSize;
	fillMethod_ = static_cast<FillMethod>(saveParam_.fillMethod);
	fillStartingPoint_ = static_cast<FillStartingPoint>(saveParam_.fillStartingPoint);

	arcData_->center = saveParam_.center;
	arcData_->fillAmount = saveParam_.fillAmount;
	arcData_->innerRadius = saveParam_.innerRadius;
	arcData_->outerRadius = saveParam_.outerRadius;
	arcData_->startAngle = saveParam_.startAngle;
	arcData_->arcRange = saveParam_.arcRange;
	arcData_->clockwise = saveParam_.clockwise;
}

void Sprite::Load(const std::string& _group, const std::string& _key) {
	SetName(_key);
	saveGroupName_ = _group;
	saveKeyName_ = _key;

	saveParam_.SetGroupName(_group);
	saveParam_.SetName(_key);
	saveParam_.Load();
	ApplyParam();
}

void Sprite::Save(const std::string& _group, const std::string& _key) {
	saveParam_.transform = transform_->GetTransform();
	saveParam_.uvTransform = uvTransform_;
	saveParam_.textureName = textureName_;
	saveParam_.color = materialData_->color;
	saveParam_.drawRange = drawRange_;
	saveParam_.leftTop = leftTop_;
	saveParam_.anchorPoint = anchorPoint_;
	saveParam_.isFlipX = isFlipX_;
	saveParam_.isFlipY = isFlipY_;
	saveParam_.spriteSize = spriteSize_;
	saveParam_.fillMethod = (int)fillMethod_;
	saveParam_.fillStartingPoint = (int)fillStartingPoint_;

	saveParam_.center = arcData_->center;
	saveParam_.fillAmount = arcData_->fillAmount;
	saveParam_.innerRadius = arcData_->innerRadius;
	saveParam_.outerRadius = arcData_->outerRadius;
	saveParam_.startAngle = arcData_->startAngle;
	saveParam_.arcRange = arcData_->arcRange;
	saveParam_.clockwise = arcData_->clockwise;

	saveGroupName_ = _group;
	saveKeyName_ = _key;
	saveParam_.SetGroupName(_group);
	saveParam_.SetName(_key);
	saveParam_.Save();
}

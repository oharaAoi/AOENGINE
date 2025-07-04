#include "ParticleSystemEditor.h"
#include "ImGuiFileDialog.h"
#include "Engine/Engine.h"
#include "Engine/Utilities/DrawUtils.h"
#include <iostream>
#include <fstream>

void ParticleSystemEditor::Finalize() {
	depthStencilResource_.Reset();
	particleRenderer_.reset();
	particlesMap_.clear();
	emitterList_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps) {
	commandList_ = commandList;
	renderTarget_ = renderTarget;
	descriptorHeaps_ = descriptorHeaps;

	// -------------------------------------------------
	// ↓ 深度バッファの作成
	// -------------------------------------------------
	depthStencilResource_ = CreateDepthStencilTextureResource(device, kClientWidth_, kClientHeight_);
	// DSVの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(depthStencilResource_.Get(), &desc, descriptorHeaps_->GetDescriptorHandle(TYPE_DSV).handleCPU);

	// -------------------------------------------------
	// ↓ Rendererの作成
	// -------------------------------------------------
	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(10000);

	camera_ = std::make_unique<EffectSystemCamera>();
	camera_->Init();

	// -------------------------------------------------
	// ↓ Editer関連
	// -------------------------------------------------
	isSave_ = false;
	isLoad_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Update() {
#ifdef _DEBUG
	// Emitterの更新
	for (auto& emitter : emitterList_) {
		emitter->Update();
	}

	// particleの更新
	ParticlesUpdate();
	// particleをRendererに送る
	particleRenderer_->SetView(camera_->GetViewMatrix() * camera_->GetProjectionMatrix(), Matrix4x4::MakeUnit());
	for (auto& particle : particlesMap_) {
		particleRenderer_->Update(particle.first, particle.second.forGpuData_, particle.second.isAddBlend);
	}

	particleRenderer_->PostUpdate();
	// カメラの更新
	camera_->Update();

	Create();
	Edit();
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Draw() {
#ifdef _DEBUG
	PreDraw();
	particleRenderer_->Draw(commandList_);
	PostDraw();
#endif // _DEBUG
}

#ifdef _DEBUG
///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Particleの更新
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::ParticlesUpdate() {
	for (auto& particles : particlesMap_) {

		size_t particleNum = particles.second.particles->size();
		particles.second.forGpuData_.resize(particleNum);
		for (uint32_t oi = 0; oi < particleNum; ++oi) {
			particles.second.forGpuData_[oi].color.w = 0.0f;
		}

		size_t index = 0;
		for (auto it = particles.second.particles->begin(); it != particles.second.particles->end();) {
			auto& pr = *it;
			// ---------------------------
			// 生存時間の更新
			// ---------------------------
			pr.lifeTime -= GameTimer::DeltaTime();
			if (pr.lifeTime <= 0.0f) {
				it = particles.second.particles->erase(it); // 削除して次の要素にスキップ
				continue;
			}

			// ---------------------------
			// Parameterの更新
			// ---------------------------
			// 速度を更新
			pr.velocity *= std::powf((1.0f - pr.damping), GameTimer::DeltaTime());

			// 重力を適応
			pr.velocity.y += pr.gravity * GameTimer::DeltaTime();

			// 座標を適応
			pr.translate += pr.velocity * GameTimer::DeltaTime();

			// ---------------------------
			// 状態の更新
			// ---------------------------
			float t = pr.lifeTime / pr.firstLifeTime;
			t = 1.0f - t;
			if (pr.isLifeOfAlpha) {
				pr.color.w = Lerp(1.0f, 0.0f, t);
			}

			if (pr.isLifeOfScale) {
				pr.scale = Vector3::Lerp(pr.firstScale, CVector3::ZERO, t);
			}

			if (pr.isScaleUpScale) {
				float scaleT = pr.lifeTime / pr.firstLifeTime;
				scaleT = 1.0f - scaleT;
				pr.scale = Vector3::Lerp(CVector3::ZERO, pr.upScale, scaleT);
			}

			Matrix4x4 scaleMatrix = pr.scale.MakeScaleMat();
			Matrix4x4 billMatrix = camera_->GetBillBordMatrix(); // ← ビルボード行列（カメラからの視線で作る）
			Matrix4x4 zRot = pr.rotate.MakeMatrix();
			Matrix4x4 rotateMatrix = Multiply(zRot, Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), billMatrix));
			Matrix4x4 translateMatrix = pr.translate.MakeTranslateMat();
			Matrix4x4 localWorld = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

			particles.second.forGpuData_[index].worldMat = localWorld;
			particles.second.forGpuData_[index].color = pr.color;

			particles.second.isAddBlend = pr.isAddBlend;

			// ---------------------------
			// NextFrameのための更新
			// ---------------------------
			++index;
			++it;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 生成する
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Create() {
	// createの準備をする
	ImGui::Begin("Create Window");
	static std::string newEffectName = "new Particles";
	char buffer[128];
	strncpy_s(buffer, sizeof(buffer), newEffectName.c_str(), _TRUNCATE);
	buffer[sizeof(buffer) - 1] = '\0'; // 安全のため null 終端

	if (ImGui::InputText("Effect Name", buffer, sizeof(buffer))) {
		newEffectName = buffer;
	}

	// createする
	if (ImGui::Button("Create")) {
		AddList(newEffectName);
	}

	// 読み込みから行う
	if (ImGui::Button("Load")) {
		isLoad_ = true;
	}

	if (isLoad_) {
		OpenLoadDialog();
	}

	ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Particleを追加する
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::AddList(const std::string& _name) {
	auto& newParticle = emitterList_.emplace_back(std::make_unique<BaseParticles>());
	newParticle->Init(_name);
	newParticle->SetShareMaterial(
		particleRenderer_->AddParticle(newParticle->GetName(),
									   newParticle->GetGeometryObject()->GetMesh(),
									   newParticle->GetIsAddBlend())
	);

	newParticle->GetShareMaterial()->SetUseTexture(newParticle->GetUseTexture());
	if (!particlesMap_.contains(_name)) {
		particlesMap_.emplace(_name, ParticleSystemEditor::ParticlesData());
	}
	newParticle->SetParticlesList(particlesMap_[_name].particles);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込むためのダイアログを開く
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::OpenLoadDialog() {
	// configデータを作成する
	IGFD::FileDialogConfig config;
	config.path = "./Game/Assets/Load/Particles/"; // 初期ディレクトリ

	// Dialogを開く
	ImGuiFileDialog::Instance()->OpenDialog(
		"LoadParticlesDialogKey",              // ダイアログ識別キー
		"Load Particles Json File",                 // ウィンドウタイトル
		".json",                           // 設定
		config                           // userDatas（不要ならnullptr）
	);

	// Dialog内の入力処理
	if (ImGuiFileDialog::Instance()->Display("LoadParticlesDialogKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			std::string nameWithoutExtension = std::filesystem::path(fileName).stem().string();

			isLoad_ = false;
			AddList(nameWithoutExtension);
			
		} else {
			// Cancel時の処理
			isLoad_ = false;
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込みを行う
///////////////////////////////////////////////////////////////////////////////////////////////

json ParticleSystemEditor::Load(const std::string& filePath) {
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "not Exist " + filePath + ".json";
		assert(0);
	}

	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	return root;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Edit() {
	// 編集したいParticleの指定を行う
	ImGui::Begin("List");
	static BaseParticles* particles = nullptr;
	static std::string openNode = "";
	for (auto& it : emitterList_) {
		BaseParticles* ptr = it.get();
		if (ImGui::Selectable(ptr->GetName().c_str(), particles == ptr)) {
			particles = it.get();
			openNode = "";  // 他のノードを閉じる
		}
	}
	ImGui::End();

	// 指定されたParticleの編集を行う
	ImGui::Begin("Setting");
	if (particles != nullptr) {
		// particle自体を編集する
		particles->Debug_Gui();

		// 外部へ出力する
		if (ImGui::CollapsingHeader("Output")) {
			if (ImGui::Button("Particles Save")) {
				isSave_ = true;
			}
		}

		if (isSave_) {
			OpenSaveDialog(particles->GetName(), particles->GetJsonData());
		}
	}
	ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Saveを行う
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::OpenSaveDialog(const std::string& _name, const json& _jsonData) {
	// configデータを作成する
	IGFD::FileDialogConfig config;
	config.path = "./Game/Assets/"; // 初期ディレクトリ
	config.fileName = _name; // 初期ファイル名
	config.flags = ImGuiFileDialogFlags_ConfirmOverwrite; // ←ここ！
	
	// Dialogを開く
	ImGuiFileDialog::Instance()->OpenDialog(
		"SaveParticlesDialogKey",              // ダイアログ識別キー
		"Save Particles Json File",                 // ウィンドウタイトル
		".json",                           // 設定
		config                           // userDatas（不要ならnullptr）
	);

	// Dialog内の入力処理
	if (ImGuiFileDialog::Instance()->Display("SaveParticlesDialogKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			std::string nameWithoutExtension = std::filesystem::path(fileName).stem().string();

			std::filesystem::path path(filePath);
			std::string directory = path.parent_path().string();

			isSave_ = false;
			Save(directory, nameWithoutExtension, _jsonData);
		} else {
			// Cancel時の処理
			isSave_ = false;
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Save(const std::string& directoryPath, const std::string& fileName, const json& jsonData) {
	if (jsonData.is_object() && !jsonData.empty()) {
		// 最上位のキーの名前をファイル名とする
		std::string rootKey = jsonData.begin().key();
		// ファイルパスの作成
		std::string filePath = directoryPath + "/" + fileName + ".json";

		// -------------------------------------------------
		// ↓ ディレクトリがなければ作成を行う
		// -------------------------------------------------
		std::filesystem::path dirPath = std::filesystem::path(directoryPath + "\\");
		if (!std::filesystem::exists(dirPath)) {
			std::filesystem::create_directories(dirPath);
			std::cout << "Created directory: " << dirPath << std::endl;
		}

		// -------------------------------------------------
		// ↓ ファイルを開けるかのチェックを行う
		// -------------------------------------------------
		std::ofstream outFile(filePath);
		if (outFile.fail()) {
			std::string message = "Faild open data file for write\n";
			//Log(message);
			assert(0);
			return;
		}

		// -------------------------------------------------
		// ↓ ファイルに実際に書き込む
		// -------------------------------------------------
		outFile << std::setw(4) << jsonData << std::endl;
		outFile.close();

		//Log("JSON data saved as: " + filePath + "\n");
	} else {
		//Log("Invalid or empty JSON data\n");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ RenderTargetをEffectEditer用にする
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::SetRenderTarget() {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = descriptorHeaps_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	dsvHandle.ptr += size_t(descriptorHeaps_->GetDescriptorSize()->GetDSV());
	// RenderTargetを指定する
	renderTarget_->SetRenderTarget(commandList_, RenderTargetType::EffectSystem_RenderTarget);
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	float clearColor[] = { 0.0f / 255, 0.0f / 255, 0.0f / 255.0f, 255.0f };
	// RenderTargetをクリアする
	commandList_->ClearRenderTargetView(renderTarget_->GetRenderTargetRTVHandle(RenderTargetType::EffectSystem_RenderTarget).handleCPU, clearColor, 0, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画前処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::PreDraw() {
	SetRenderTarget();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin("ParticleSystemEditor", nullptr,
				 ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoBackground);

	// Grid線描画
	DrawGrid(camera_->GetViewMatrix(), camera_->GetProjectionMatrix());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画後処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::PostDraw() {
	Engine::SetPSOPrimitive();
	Render::PrimitiveDrawCall();

	// 最後にImGui上でEffectを描画する
	renderTarget_->TransitionResource(commandList_, EffectSystem_RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	ImTextureID textureID2 = reinterpret_cast<ImTextureID>(static_cast<uint64_t>(renderTarget_->GetRenderTargetSRVHandle(RenderTargetType::EffectSystem_RenderTarget).handleGPU.ptr));
	ImGui::SetCursorPos(ImVec2(20, 60)); // 描画位置を設定
	ImGui::Image((void*)textureID2, ImVec2(640.0f, 360.0f)); // サイズは適宜調整

	ImGui::End();
	ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::End() {
	renderTarget_->TransitionResource(commandList_, EffectSystem_RenderTarget, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

#endif // _DEBUG
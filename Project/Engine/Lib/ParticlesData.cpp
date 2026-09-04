#include "ParticlesData.h"
#include "Engine/System/Manager/ImGuiManager.h"

using namespace AOENGINE;

void ParticleEmit::Attribute_Gui() {
	if (ImGui::CollapsingHeader("エミッター設定")) {
		ImGui::Checkbox("ループ再生", &isLoop);
		ImGui::DragFloat("再生時間（秒）", &duration, 0.1f, 0.0f);
		ImGui::Checkbox("ビルボード表示", &isBillBord);
		ImGui::BulletText("発生位置・形状");
		ImGui::DragFloat3("回転", &rotate.x, 0.01f);
		ImGui::DragFloat3("位置", (float*)&translate, 0.1f);
		ImGui::DragScalar("1秒あたりの生成数", ImGuiDataType_U32, &rateOverTimeCout, 1.0f);
		ImGui::DragFloat("移動時の生成間隔", &emitSpacing, 0.01f, 0.001f);
		ImGui::DragScalar("最大パーティクル数", ImGuiDataType_U32, &maxParticles, 1.0f, nullptr, nullptr);
		ImGui::DragScalar("1フレームの最大生成数", ImGuiDataType_U32, &maxEmitPerFrame, 1.0f, nullptr, nullptr);
		ImGui::DragFloat("テレポート判定距離", &teleportThreshold, 0.1f, 0.0f);
		ImGui::Combo("上限到達時", &overflowMode, "新規生成を停止\0最も古いものを再利用\0");

		ImGui::Combo("発生形状", &shape, "球\0ボックス\0コーン\0");
		if (shape == (int)CpuEmitterShape::Shpere) {
			ImGui::DragFloat("半径", &radius, 0.1f, 0.0f);
		} else if (shape == (int)CpuEmitterShape::Box) {
			ImGui::DragFloat3("範囲サイズ", &size.x, 0.1f);
		} else if (shape == (int)CpuEmitterShape::Cone) {
			ImGui::DragFloat("半径", &radius, 0.1f, 0.0f);
			ImGui::DragFloat("広がり角度", &angle, 0.1f, 0.0f, 89.0f);
			ImGui::DragFloat("長さ", &height, 0.1f, 0.0f);
			ImGui::Combo("コーンの射出元", &coneEmitFrom, "底面\0体積\0");
			ImGui::SliderFloat("半径の厚み", &radiusThickness, 0.0f, 1.0f);
			ImGui::SliderFloat("円周範囲", &arc, 0.0f, 360.0f, "%.1f 度");
			ImGui::SliderFloat("方向のランダム度", &randomDirectionAmount, 0.0f, 1.0f);
		}
		ImGui::Combo("生成位置", &emitOrigin, "中心\0範囲内\0");
		ImGui::Combo("射出方向", &emitDirection, "上方向\0ランダム\0外向き\0中心向き\0");
		ImGui::Separator();

		ImGui::BulletText("描画・カラー");
		int type = (int)blendModeType;
		ImGui::Combo("ブレンドモード", &type, "なし\0通常\0加算\0減算\0乗算\0スクリーン\0");
		blendModeType = (uint32_t)type;
		ImGui::Checkbox("生成色をランダムにする", &isRandomColor);
		if (isRandomColor) {
			ImGui::ColorEdit4("ランダム色A", (float*)&randColor1);
			ImGui::ColorEdit4("ランダム色B", (float*)&randColor2);
		} else {
			ImGui::ColorEdit4("基本色", (float*)&color);
		}
		ImGui::Checkbox("破棄しきい値を時間変化", &isLerpDiscardValue);
		if (!isLerpDiscardValue) {
			ImGui::DragFloat("破棄しきい値", &discardValue, 0.01f, 0.0f, 1.0f);
		} else {
			ImGui::DragFloat("開始時の破棄しきい値", &startDiscard, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("終了時の破棄しきい値", &endDiscard, 0.01f, 0.0f, 1.0f);
		}

		ImGui::BulletText("サイズ・動き");
		ImGui::Checkbox("軸ごとにサイズを設定", &separateByAxisScale);
		if (separateByAxisScale) {
			ImGui::DragFloat3("最小サイズ", &minScale.x, 0.1f);
			ImGui::DragFloat3("最大サイズ", &maxScale.x, 0.1f);
		} else {
			ImGui::DragFloat("最小サイズ", &minScale.x);
			ImGui::DragFloat("最大サイズ", &maxScale.x);
		}
		ImGui::DragFloat("初速度", &speed, 0.01f);
		ImGui::DragFloat("寿命（秒）", &lifeTime, 0.01f, 0.01f);
		ImGui::DragFloat("重力", &gravity, 0.01f);
		ImGui::DragFloat("減衰率", &dampig, 0.01f);
		ImGui::Text("初期角度");
		ImGui::DragFloat("最小角度", &angleMin, 0.01f);
		ImGui::DragFloat("最大角度", &angleMax, 0.01f);
		ImGui::Text("寿命による変化");

		ImGui::Checkbox("進行方向に回転", &isDirectionRotate);
		ImGui::Checkbox("寿命でサイズを変化", &isLifeOfScale);
		if (isLifeOfScale) {
			ImGui::DragFloat3("開始サイズ", &lifeOfMinScale.x, 0.1f);
			ImGui::DragFloat3("終了サイズ", &lifeOfMaxScale.x, 0.1f);
		}
		ImGui::Checkbox("寿命で透明にする", &isLifeOfAlpha);
		ImGui::Checkbox("フェードイン・アウト", &isFadeInOut);
		if (isFadeInOut) {
			ImGui::DragFloat("フェードイン時間", &fadeInTime, 0.1f, 0.0f);
			ImGui::DragFloat("フェードアウト時間", &fadeOutTime, 0.1f, 0.0f);
		}

		ImGui::Checkbox("速度方向に引き伸ばす", &isStretch);
		ImGui::Checkbox("2Dとして描画", &isDraw2d);
		ImGui::Checkbox("ゼロから拡大", &isScaleUp);
		if (isScaleUp) {
			ImGui::DragFloat3("拡大後のサイズ", (float*)&scaleUpScale, 0.01f);
		}

		ImGui::Checkbox("##isTextureSheetAnimation", &isTextureSheetAnimation);
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("テクスチャシートアニメーション")) {
			ImGui::DragFloat2("分割数", &tiles.x, 1.0f);
		}

		ImGui::Checkbox("##isColorAnimation", &isColorAnimation);
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("カラーグラデーション")) {
			ImGui::ColorEdit4("開始色", &preColor.r);
			ImGui::ColorEdit4("終了色", &postColor.r);
		}

		ImGui::Checkbox("##isRandomRotate", &isRandomRotate);
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("ランダム回転")) {
			ImGui::DragFloat("回転の最小角度", &minAngle);
			ImGui::DragFloat("回転の最大角度", &maxAngle);
		}

		minScale.Clamp(minScale, maxScale);
	}
}

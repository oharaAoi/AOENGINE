#pragma once
#include <string>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Quaternion.h"

/// <summary>
/// CPUで行うEmitter
/// </summary>
class CpuEmitter {
public:

	/// <summary>
	/// 射出する形状の情報
	/// </summary>
	struct EmitParameter {
		Vector4 rotate;			// 回転(Quaternion)
		Vector3 translate;		// 位置
		uint32_t shape;			// emitterの種類
		uint32_t count;			// 射出数
		float frequency;		// 射出間隔
		float frequencyTime;	// 時間調整用(実際に動かす時間)
		int emit;				// 射出許可

		// particle自体のparameter
		Vector4 color;			// 色
		Vector3 minScale;		// 最小の大きさ
		Vector3 maxScale;		// 最大の大きさ
		float speed;			// 速度
		float lifeTime;			// particleの生存時間
		float gravity;			// 重力を付与するか
		float dampig;			// 減衰率
	};

	struct EmitterParameter {
		std::string particleShape;
		std::string materialTexture;
		Vector3 velocity;	// 速度
		float speed;		// 移動速度
		float lifeTime;		// 生存時間
		bool oneShot;		// 
	};

public:

	CpuEmitter();
	~CpuEmitter();

	void Init();
	void Update();
	void DrawShape();

#ifdef _DEBUG
	void Debug_Gui();
#endif

private:

	EmitParameter emitParameter_;
	EmitterParameter emitterParametr_;

	bool isDead_ = false;
	bool isMove_ = false;

	std::string particleShape_;
	std::string materialTexture_;

};


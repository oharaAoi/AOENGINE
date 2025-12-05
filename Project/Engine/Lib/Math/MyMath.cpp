#include "MyMath.h" 

using namespace Math;

Math::Vector3 DegToRad(const Math::Vector3& rotate) {
	return Math::Vector3(rotate.x * kToRadian, rotate.y * kToRadian, rotate.z * kToRadian);
}

Math::Vector3 RadToDeg(const Math::Vector3& rotate) {
	return Math::Vector3(rotate.x * kToDegree, rotate.y * kToDegree, rotate.z * kToDegree);
}

float Clamp01(float value) {
	return std::clamp(value, 0.f, 1.f);
}

float Length(const Math::Vector3& vec3) {
	return std::sqrt(vec3.x * vec3.x + vec3.y * vec3.y + vec3.z * vec3.z);
}

float Distance(const Math::Vector3& v1, const Math::Vector3& v2) {
	return Length(v1 - v2);
}

float Normalize(float value, float min, float max) {
	return (value - min) / (max - min);
}

Math::Vector3 Normalize(const Math::Vector3& vec3) {
	Math::Vector3 result = vec3;

	float len = Length(vec3);
	if (len != 0) {
		result.x /= len;
		result.y /= len;
		result.z /= len;
	}

	return result;
}

float Dot(const Math::Vector3& v1, const Math::Vector3& v2) {
	float result{};
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return result;
}

Math::Vector3 Cross(const Math::Vector3& v1, const Math::Vector3& v2) {
	Math::Vector3 result{};
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

Math::Vector3 Projection(const Math::Vector3& v1, const Math::Vector3& v2) {
	Math::Vector3 result{};
	float middle{};
	Math::Vector3 unitVector = Normalize(v2);

	middle = Dot(v1, unitVector);

	result = unitVector * middle;

	return result;
}

Math::Vector3 ClosestPoint(const Math::Vector3& point, const Math::Line& segment) {
	Math::Vector3 result{};
	// 正射影ベクトルを求める
	Math::Vector3 projVector = Projection(point - segment.origin, segment.diff);
	// 最近接点を求める
	result = segment.origin + projVector;
	return result;
}

float Lerp(float v1, float v2, float t) {
	return std::lerp(v1, v2, t);
}

Math::Vector2 Lerp(const Math::Vector2& v1, const Math::Vector2& v2, float t) {
	Math::Vector2 result{};
	result.x = std::lerp(v1.x, v2.x, t);
	result.y = std::lerp(v1.y, v2.y, t);

	return result;
}

Math::Vector3 Lerp(const Math::Vector3& v1, const Math::Vector3& v2, float t) {
	Math::Vector3 result{};
	result.x = std::lerp(v1.x, v2.x, t);
	result.y = std::lerp(v1.y, v2.y, t);
	result.z = std::lerp(v1.z, v2.z, t);

	//v1 + (v2 - v1) * t;

	return result;
}

Math::Vector4 Lerp(const Math::Vector4& v1, const Math::Vector4& v2, float t) {
	Math::Vector4 result{};
	result.x = std::lerp(v1.x, v2.x, t);
	result.y = std::lerp(v1.y, v2.y, t);
	result.z = std::lerp(v1.z, v2.z, t);
	result.w = std::lerp(v1.w, v2.w, t);
	return result;
}

Color Lerp(const Color& v1, const Color& v2, float t) {
	Color result{};
	result.r = std::lerp(v1.r, v2.r, t);
	result.g = std::lerp(v1.g, v2.g, t);
	result.b = std::lerp(v1.b, v2.b, t);
	result.a = std::lerp(v1.a, v2.a, t);
	return result;
}

Math::Vector3 Slerp(const Math::Vector3& v1, const Math::Vector3& v2, float t) {
	Math::Vector3 result{};
	// なす角を求める
	float theta = Angle(v1, v2);

	// 引数のベクトルのlerpを求める
	float lerp = (1 - t) * Length(v1) + (t * Length(v2)); // s

	// 引数のベクトルの単位ベクトルを求める
	Math::Vector3 unitV1 = Normalize(v1);    // ea
	Math::Vector3 unitV2 = Normalize(v2);    // eb

	// ベクトルの角度を求める
	float midTheta1;
	float midTheta2;

	// なす角が0の時はそのままの角度を返すようにする
	if (theta == 0) {
		midTheta1 = 1;
		midTheta2 = 1;
	} else {
		midTheta1 = std::sin((1 - t) * theta) / std::sin(theta);
		midTheta2 = std::sin(t * theta) / std::sin(theta);
	}

	// 角度からベクトルを求める
	Math::Vector3 midV1 = unitV1 * midTheta1;
	Math::Vector3 midV2 = unitV2 * midTheta2;

	Math::Vector3 mid = midV1 + midV2;

	// 正規の補完分と求めたベクトルを掛ける
	result = mid * lerp;

	return result;
}

float LerpShortAngle(const float& originalAngle, const float& targetAngle, const float& t) {
	float diff = targetAngle - originalAngle;

	// 角度を-2π~2πに補完
	diff = std::fmodf(diff, std::numbers::pi_v<float> *2.0f);
	// 角度を-π~πに補完
	if (diff > std::numbers::pi_v<float>) {
		diff = diff - (std::numbers::pi_v<float> *2.0f);

	} else if (diff < -std::numbers::pi_v<float>) {
		diff = diff + (std::numbers::pi_v<float> *2.0f);
	}


	return originalAngle + (diff * t);
}

float Angle(const Math::Vector3& v1, const Math::Vector3& v2) {
	float result;

	// ベクトル長さを求める
	float normV1 = Length(v1);
	float normV2 = Length(v2);

	if (normV1 == 0 || normV2 == 0) {
		return 0;
	}
	// 内積
	float dot = Dot(v1, v2);

	// cosを求める
	float cosTheta = dot / (normV1 * normV2);

	result = std::acos(cosTheta);

	return result;
}

/// <summary>
/// 臨界減衰スプリング
/// </summary>
/// <param name="current"></param>
/// <param name="target"></param>
/// <param name="currentVelocity"></param>
/// <param name="smoothTime"></param>
/// <param name="maxSpped"></param>
/// <param name="deltaTime"></param>
/// <returns></returns>
float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
	smoothTime = std::clamp(smoothTime, 0.0001f, smoothTime);
	float omega = 2.0f / smoothTime;

	float x = omega * deltaTime;
	float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

	float change = current - target;
	float originalTo = target;

	// Clamp max speed
	float maxChange = maxSpeed * smoothTime;
	change = std::clamp(change, -maxChange, maxChange);
	target = current - change;

	float temp = (currentVelocity + omega * change) * deltaTime;
	currentVelocity = (currentVelocity - omega * temp) * exp;

	float output = target + (change + temp) * exp;

	// Prevent overshoot
	if ((originalTo - current > 0.0f) == (output > originalTo)) {
		output = originalTo;
		currentVelocity = (output - originalTo) / deltaTime;
	}

	return output;
}

Math::Vector3 SmoothDamp(const Math::Vector3& current, const Math::Vector3& target, Math::Vector3& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
	return Math::Vector3(
		SmoothDamp(current.x, target.x, currentVelocity.x, smoothTime, maxSpeed, deltaTime),
		SmoothDamp(current.y, target.y, currentVelocity.y, smoothTime, maxSpeed, deltaTime),
		SmoothDamp(current.z, target.z, currentVelocity.z, smoothTime, maxSpeed, deltaTime)
	);
}

/// <summary>///
/// CatmullRom補完
/// </summary>///
/// <param name="p0">点0の座標</param>///
/// <param name="p1">点1の座標</param>///
/// <param name="p2">点2の座標</param>///
/// <param name="p3">点3の座標</param>///
/// <param name="t">点1と点2の間で指定された座標</param>///
/// <returns>点1と点2の間で指定された座標</returns>
Math::Vector3 CatmullRomInterpolation(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& p2, const Math::Vector3& p3, float t) {
	const float s = 0.5f; // 数式に出てくる 1/2のこと

	float t2 = t * t; // tの2乗
	float t3 = t2 * t; // tの3乗

	// ベクトル値関数の式
	Math::Vector3 e3 = (p0 * -1) + (p1 * 3) - (p2 * 3) + p3;
	Math::Vector3 e2 = (p0 * 2) - (p1 * 5) + (p2 * 4) - p3;
	Math::Vector3 e1 = (p0 * -1) + p2;
	Math::Vector3 e0 = p1 * 2;

	return ((e3 * t3) + (e2 * t2) + (e1 * t) + e0) * s;
}

/// <summary>///
/// CatmullRomスプライン曲線上の座標を得る
/// </summary>///
/// <param name="points">制御点の集合</param>///
/// <param name="t">スプライン全区間の中で割合指定</param>///
/// <returns>座標</returns>
Math::Vector3 CatmullRomPosition(const std::vector<Math::Vector3>& points, float t) {
	assert(points.size() >= 4 && "制御点は4以上必要です");
	// 区間数は制御点の数の-1
	size_t division = points.size() - 1;
	// 1区間の長さ(全体を1.0とした割合)
	float areaWidth = 1.0f / division;

	// 区間の始点を0.0f, 終点を1.0fとしたときの現在の位置
	float t2 = std::fmod(t, areaWidth) * division;
	// 下限(0.0f)と上限(1.0f)の範囲に収める
	t2 = std::clamp(t2, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	// 区間番号が上限を超えないように収める
	index = std::clamp(static_cast<int>(index), 0, static_cast<int>(division) - 1);

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 始点の場合はp0とp1は同じ
	if (index == 0) {
		index0 = index1;
	}
	// 最後の区間はp3とp2は同じ
	if (index3 >= points.size()) {
		index3 = index2;
	}

	// 4点の座標
	const Math::Vector3& p0 = points[index0];
	const Math::Vector3& p1 = points[index1];
	const Math::Vector3& p2 = points[index2];
	const Math::Vector3& p3 = points[index3];

	return CatmullRomInterpolation(p0, p1, p2, p3, t2);
}

/// <summary>
/// ベジエ曲線を書く
/// </summary>
/// <param name="controlPoint">制御点がまとまった配列</param>
/// <returns></returns>
Math::Vector3 Bezier(const std::vector<Math::Vector3>& controlPoint, const float& t) {
	if (controlPoint.size() == 1) {
		return controlPoint[0];
	}

	std::vector<Math::Vector3> lerpVec;
	for (size_t i = 0; i < controlPoint.size() - 1; ++i) {
		lerpVec.push_back(Lerp(controlPoint[i], controlPoint[i + 1], t));
	}

	return Bezier(lerpVec, t);
}


/// <summary>
/// スクリーン座標からワールド座標に変換する関数
/// </summary>
/// <param name="screenPos">:スクリーン上の座標</param>
/// <param name="inverseWvpMat">:逆ワールドプロジェクション行列</param>
/// <param name="setDirection">:ワールド上のどこに設定するか</param>
/// <returns>ワールド座標</returns>
Math::Vector3 ScreenToWorldCoordinate(const Math::Vector2& screenPos, const Math::Matrix4x4& inverseWvpMat, const float& setDirection) {
	// ニアとファーを求める
	Math::Vector3 posNear = Math::Vector3(screenPos.x, screenPos.y, 0);
	Math::Vector3 posFar = Math::Vector3(screenPos.x, screenPos.y, 1);

	//　スクリーン座標系からワールド座標形に
	posNear = Transform(posNear, inverseWvpMat);
	posFar = Transform(posFar, inverseWvpMat);

	// ニアの点からファーの点に向かう光線を求める
	Math::Vector3 mouseDirection = posFar - posNear;
	mouseDirection = Normalize(mouseDirection);

	// 結果
	Math::Vector3 result = posNear + (mouseDirection * setDirection);

	return result;
}

/// <summary>
/// スクリーン座標からワールド座標に変換する関数
/// </summary>
/// <param name="screenPos">スクリーン上の座標</param>
/// <param name="inverseWvpMat">逆ワールドプロジェクション行列</param>
/// <param name="setDirection">ワールド上のどこに設定するか</param>
/// <returns>ワールド座標</returns>
Math::Vector2 WorldToScreenCoordinate(const Math::Matrix4x4& targetWorldMat, const Math::Matrix4x4& vpvpMat) {
	Math::Vector3 pos = Transform(CVector3::ZERO, targetWorldMat * vpvpMat);
	return Math::Vector2(pos.x, pos.y);
}

/// <summary>
/// ベクトル変換
/// </summary>
/// <param name="v"></param>
/// <param name="m"></param>
/// <returns></returns>
Math::Vector3 TransformNormal(const Math::Vector3& v, const Math::Matrix4x4& m) {
	Math::Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};

	return result;
}

/// <summary>
/// ベジエ曲線を書く
/// </summary>
/// <param name="controlPoint">制御点がまとまった配列</param>
/// <returns></returns>
Math::Vector3 Bezier(const Math::Vector3& v1, const Math::Vector3& v2, const Math::Vector3& v3, const float& t) {

	Math::Vector3 lerpVec[2]{};
	lerpVec[0] = (v2 - v1).Normalize() * Length(v2 - v1) * t;
	lerpVec[1] = (v3 - v2).Normalize() * Length(v3 - v2) * t;
	return (lerpVec[1] - lerpVec[0]).Normalize() * Length(lerpVec[1] - lerpVec[0]) * t;
}


uint32_t Red(uint32_t color) { return (color >> 24) & 0xFF; }
uint32_t Green(uint32_t color) { return (color >> 16) & 0xFF; }
uint32_t Blue(uint32_t color) { return (color >> 8) & 0xFF; }
uint32_t Alpha(uint32_t color) { return color & 0xFF; }


// RGBA形式のカラーコードをMath::Vector4形式に変換する関数 (各要素は0~1に収まる)
Math::Vector4 FloatColor(uint32_t color) {
	float delta = 1.0f / 255.0f;

	Math::Vector4 colorf = {
		float(Red(color)) * delta,
		float(Green(color)) * delta,
		float(Blue(color)) * delta,
		float(Alpha(color)) * delta
	};

	return colorf;
}


uint32_t IntColor(const Math::Vector4& color) {
	uint32_t red = std::clamp(int(color.x * 255.0f), 0, 255) << 24;
	uint32_t green = std::clamp(int(color.y * 255.0f), 0, 255) << 16;
	uint32_t blue = std::clamp(int(color.z * 255.0f), 0, 255) << 8;
	uint32_t alpha = std::clamp(int(color.w * 255.0f), 0, 255);

	return red + green + blue + alpha;
}

uint32_t HSV_to_RGB(float h, float s, float v, float alpha) {

	// 彩度が0なので明度のみを反映
	if (s == 0.0) {
		return IntColor(Math::Vector4(v, v, v, alpha));
	}

	h *= 6.0;
	int i = int(h);
	float f = h - i;

	float p = v * (1.0f - s);
	float q = v * (1.0f - s * f);
	float t = v * (1.0f - s * (1.0f - f));

	if (i % 6 == 0) {
		return  IntColor(Math::Vector4(v, t, p, alpha));
	} else if (i % 6 == 1) {
		return  IntColor(Math::Vector4(q, v, p, alpha));
	} else if (i % 6 == 2) {
		return  IntColor(Math::Vector4(p, v, t, alpha));
	} else if (i % 6 == 3) {
		return  IntColor(Math::Vector4(p, q, v, alpha));
	} else if (i % 6 == 4) {
		return  IntColor(Math::Vector4(t, p, v, alpha));
	} else {
		return  IntColor(Math::Vector4(v, p, q, alpha));
	}
}
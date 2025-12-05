#include "Quaternion.h"
#include "Engine/Lib/Math/MyMath.h"

using namespace Math;

Math::Quaternion Math::Quaternion::Normalize() const {
	float length = std::sqrt(x * x + y * y + z * z + w * w);
	if (length < 1e-6f) { // 長さが非常に小さい場合
		return Math::Quaternion(0, 0, 0, 1); // デフォルト値を返す
	}
	float invLength = 1.0f / length;
	return Math::Quaternion(x * invLength, y * invLength, z * invLength, w * invLength);
}

Math::Matrix4x4 Math::Quaternion::MakeMatrix() const {
	// クォータニオンの各成分
	Math::Matrix4x4 result = Math::Matrix4x4::MakeUnit();
	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	float ww = w * w;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;

	result.m[0][0] = ww + xx - yy - zz;
	result.m[0][1] = 2.0f * (xy + wz);
	result.m[0][2] = 2.0f * (xz - wy);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (xy - wz);
	result.m[1][1] = ww - xx + yy - zz;
	result.m[1][2] = 2.0f * (yz + wx);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (xz + wy);
	result.m[2][1] = 2.0f * (yz - wx);
	result.m[2][2] = ww - xx - yy + zz;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Math::Quaternion Math::Quaternion::AngleAxis(float angle, const Math::Vector3& axis) {
	Math::Vector3 normalizeAxis = axis.Normalize();
	//float sinHalfAngle = std::sin(angle / 2.0f);

	return Math::Quaternion(
		normalizeAxis.x * std::sin(angle / 2.0f),
		normalizeAxis.y * std::sin(angle / 2.0f),
		normalizeAxis.z * std::sin(angle / 2.0f),
		std::cos(angle / 2.0f)
	);
}

Math::Quaternion Math::Quaternion::EulerToQuaternion(const Math::Vector3& euler) {
	float cy = std::cos(euler.y * 0.5f); // yaw
	float sy = std::sin(euler.y * 0.5f);
	float cp = std::cos(euler.x * 0.5f); // pitch
	float sp = std::sin(euler.x * 0.5f);
	float cr = std::cos(euler.z * 0.5f); // roll
	float sr = std::sin(euler.z * 0.5f);

	Math::Quaternion q;
	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;
	return q;
}

Math::Quaternion Math::Quaternion::FromToRotation(const Math::Vector3& fromDire, const Math::Vector3& toDire) {
	Math::Vector3 cross = Math::Vector3::Cross(fromDire, toDire);  // クロス積
	float dot = Math::Vector3::Dot(fromDire, toDire);        // ドット積

	// 回転角度
	float angle = std::acos(dot);

	// 回転軸
	if (cross.x == 0 && cross.y == 0 && cross.z == 0) {
		cross = Math::Vector3(fromDire.y, -fromDire.x, 0.0f);
	}
	Math::Vector3 axis = cross.Normalize();

	// クォータニオンを計算
	float sinHalfAngle = std::sin(angle / 2.0f);
	float cosHalfAngle = std::cos(angle / 2.0f);

	return Math::Quaternion(axis.x * sinHalfAngle, axis.y * sinHalfAngle, axis.z * sinHalfAngle, cosHalfAngle);
}

Math::Quaternion Math::Quaternion::Inverse(const Math::Quaternion& rotation) {
	float norm = rotation.w * rotation.w + rotation.x * rotation.x + rotation.y * rotation.y + rotation.z * rotation.z;
	return Math::Quaternion(-rotation.x / norm, -rotation.y / norm, -rotation.z / norm, rotation.w / norm);
}

Math::Quaternion Math::Quaternion::LookRotation(const Math::Vector3& forward, const Math::Vector3& upVector) {
	// 正面からforwardに向ける回転を取得
	Math::Quaternion lookRotation = FromToRotation(Math::Vector3{ 0.0f, 0.0f, 1.0f }, forward.Normalize());
	// 外積を用いてupVectorとforwardに垂直なベクトルを求める
	Math::Vector3 xAxisHorizontal = Cross(upVector, forward).Normalize();
	if (xAxisHorizontal.Length() < 1e-6) {
		// forwardとupVectorが平行な場合、回転が不定になるため例外処理やデフォルト回転を返す
		return lookRotation;
	}
	// 回転後のy軸を求める
	Math::Vector3 yAxisAfterRotate = Cross(forward, xAxisHorizontal).Normalize();
	// Look後のy軸から回転後のy軸へ修正する回転を求める
	Math::Vector3 yAxisVBefore = (lookRotation * Math::Vector3{ 0.0f, 1.0f, 0.0f }).Normalize();
	Math::Quaternion modifyRotation = FromToRotation(yAxisVBefore, yAxisAfterRotate);
	return modifyRotation * lookRotation;
}

Math::Quaternion Math::Quaternion::LookAt(const Math::Vector3& from, const Math::Vector3& to, const Math::Vector3& up) {
	Math::Vector3 forward = (to - from).Normalize();
	return LookRotation(forward, up).Normalize();
}

float Math::Quaternion::Dot(const Math::Quaternion& q1, const Math::Quaternion& q2) {
	return (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);
}

Math::Quaternion Math::Quaternion::Slerp(const Math::Quaternion& q1, const Math::Quaternion& q2, const float& t) {
	float dot = Dot(q1, q2);
	dot = std::clamp(dot, -1.0f, 1.0f);

	Math::Quaternion newQ1 = q1;
	if (dot < 0.0f) {
		newQ1 = Math::Quaternion(-q1.x, -q1.y, -q1.z, -q1.w);
		dot = -dot;
	}

	if (dot > 0.9995f) { // 線形補間 (Lerp) を使用
		Math::Quaternion lerpResult = Math::Quaternion(
			(1.0f - t) * newQ1.x + t * q2.x,
			(1.0f - t) * newQ1.y + t * q2.y,
			(1.0f - t) * newQ1.z + t * q2.z,
			(1.0f - t) * newQ1.w + t * q2.w
		);
		return lerpResult.Normalize(); // 正規化
	}

	float rad = std::acos(dot); // 内積に基づいて角度を取得
	float bottom = std::sin(rad); // sinθ
	if (std::fabs(bottom) < 1e-6f) {
		Math::Quaternion lerpResult = Math::Quaternion(
			(1.0f - t) * newQ1.x + t * q2.x,
			(1.0f - t) * newQ1.y + t * q2.y,
			(1.0f - t) * newQ1.z + t * q2.z,
			(1.0f - t) * newQ1.w + t * q2.w
		);
		return lerpResult.Normalize(); // 正規化
	}

	float a_rotate = std::sin((1.0f - t) * rad) / bottom;
	float b_rotate = std::sin(t * rad) / bottom;

	Math::Quaternion result = Math::Quaternion(
		(newQ1.x * a_rotate) + (q2.x * b_rotate),
		(newQ1.y * a_rotate) + (q2.y * b_rotate),
		(newQ1.z * a_rotate) + (q2.z * b_rotate),
		(newQ1.w * a_rotate) + (q2.w * b_rotate)
	);

	return result.Normalize();
}

Math::Quaternion Math::Quaternion::RotateTowards(const Math::Quaternion& q1, const Math::Quaternion& q2, float maxDegreesDelta) {
	float angle = Angle(q1, q2); // fromとtoの間の角度を取得

	// 角度が0またはmaxDegreesDelta以上なら補間
	if (angle == 0.0f) {
		return q2;
	}

	float t = std::clamp(maxDegreesDelta / angle, 0.0f, 1.0f);

	return Slerp(q1, q2, t);
}

Math::Quaternion Math::Quaternion::EulerToQuaternion(float pitch, float yaw, float roll) {
	float halfPitch = pitch * 0.5f;
	float halfYaw = yaw * 0.5f;
	float halfRoll = roll * 0.5f;

	Math::Quaternion qYaw = Math::Quaternion(0, std::sinf(halfYaw), 0, std::cosf(halfYaw));
	Math::Quaternion qPitch = Math::Quaternion(std::sinf(halfPitch), 0, 0, std::cosf(halfPitch));
	Math::Quaternion qRoll = Math::Quaternion(0, 0, std::sinf(halfRoll), std::cosf(halfRoll));


	return qYaw * qPitch * qRoll;
}

Math::Vector3 Math::Quaternion::QuaternionToEuler() const {
	Math::Quaternion q = this->Normalize();
	Math::Vector3 euler;

	// Roll (Z軸回転)
	float sinr_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosr_cosp = 1.0f - 2.0f * (q.z * q.z + q.x * q.x);
	euler.z = std::atan2(sinr_cosp, cosr_cosp);

	// Pitch (X軸回転)
	float sinp = 2.0f * (q.w * q.x - q.y * q.z);
	if (std::abs(sinp) >= 1.0f)
		euler.x = std::copysign(kPI / 2.0f, sinp); // ±90度にクランプ
	else
		euler.x = std::asin(sinp);

	// Yaw (Y軸回転)
	float siny_cosp = 2.0f * (q.w * q.y + q.z * q.x);
	float cosy_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	euler.y = std::atan2(siny_cosp, cosy_cosp);

	return euler;
}

Math::Quaternion Math::Quaternion::ToQuaternion(const Math::Vector4& v) {
	return Math::Quaternion(v.x, v.y, v.z, v.w);
}

Math::Quaternion Math::Quaternion::FromMatrix(const Math::Matrix4x4& m) {
	Math::Quaternion q;
	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2]; // 対角成分の合計（トレース）

	if (trace > 0.0f) {
		float s = sqrtf(trace + 1.0f) * 2.0f; // s = 4 * qw
		q.w = 0.25f * s;
		q.x = (m.m[2][1] - m.m[1][2]) / s;
		q.y = (m.m[0][2] - m.m[2][0]) / s;
		q.z = (m.m[1][0] - m.m[0][1]) / s;
	} else {
		if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]) {
			float s = sqrtf(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f; // s = 4 * qx
			q.w = (m.m[2][1] - m.m[1][2]) / s;
			q.x = 0.25f * s;
			q.y = (m.m[0][1] + m.m[1][0]) / s;
			q.z = (m.m[0][2] + m.m[2][0]) / s;
		} else if (m.m[1][1] > m.m[2][2]) {
			float s = sqrtf(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f; // s = 4 * qy
			q.w = (m.m[0][2] - m.m[2][0]) / s;
			q.x = (m.m[0][1] + m.m[1][0]) / s;
			q.y = 0.25f * s;
			q.z = (m.m[1][2] + m.m[2][1]) / s;
		} else {
			float s = sqrtf(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f; // s = 4 * qz
			q.w = (m.m[1][0] - m.m[0][1]) / s;
			q.x = (m.m[0][2] + m.m[2][0]) / s;
			q.y = (m.m[1][2] + m.m[2][1]) / s;
			q.z = 0.25f * s;
		}
	}

	return q;
}

Math::Quaternion Math::Quaternion::RotateMatrixTo(const Math::Matrix3x3 m) {
	Math::Quaternion q;
	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	if (trace > 0.0f) {
		float s = sqrtf(trace + 1.0f) * 2.0f;
		q.w = 0.25f * s;
		q.x = (m.m[2][1] - m.m[1][2]) / s;
		q.y = (m.m[0][2] - m.m[2][0]) / s;
		q.z = (m.m[1][0] - m.m[0][1]) / s;
	} else if ((m.m[0][0] > m.m[1][1]) && (m.m[0][0] > m.m[2][2])) {
		float s = sqrtf(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f;
		q.w = (m.m[2][1] - m.m[1][2]) / s;
		q.x = 0.25f * s;
		q.y = (m.m[0][1] + m.m[1][0]) / s;
		q.z = (m.m[0][2] + m.m[2][0]) / s;
	} else if (m.m[1][1] > m.m[2][2]) {
		float s = sqrtf(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f;
		q.w = (m.m[0][2] - m.m[2][0]) / s;
		q.x = (m.m[0][1] + m.m[1][0]) / s;
		q.y = 0.25f * s;
		q.z = (m.m[1][2] + m.m[2][1]) / s;
	} else {
		float s = sqrtf(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f;
		q.w = (m.m[1][0] - m.m[0][1]) / s;
		q.x = (m.m[0][2] + m.m[2][0]) / s;
		q.y = (m.m[1][2] + m.m[2][1]) / s;
		q.z = 0.25f * s;
	}

	return q;
}

Math::Vector3 Math::Quaternion::MakeForward() const {
	float xx = x * x;
	float yy = y * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	return  Math::Vector3(2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - (2.0f * (xx + yy))).Normalize();
}

Math::Vector3 Math::Quaternion::MakeUp() const {
	float xx = x * x;
	float zz = z * z;
	float xy = x * y;
	float yz = y * z;
	float wx = w * x;
	float wz = w * z;
	return Math::Vector3(2.0f * (xy - wz), 1 - (2.0f * (xx + zz)), 2.0f * (yz + wx)).Normalize();
}

Math::Vector3 Math::Quaternion::MakeRight() const {
	// 標準的な右方向ベクトル (1, 0, 0)
	Math::Vector3 right(1.0f, 0.0f, 0.0f);
	Math::Quaternion result = (*this);

	return (result * right);
}

Math::Vector3 Math::Quaternion::ToEulerAngles() const {
	Math::Vector3 euler;

	// pitch (x軸回転)
	float sinr_cosp = 2.0f * (w * x + y * z);
	float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
	euler.x = std::atan2(sinr_cosp, cosr_cosp);

	// yaw (y軸回転)
	float sinp = 2.0f * (w * y - z * x);
	if (std::fabs(sinp) >= 1)
		euler.y = std::copysign(kPI / 2.0f, sinp); // クランプ
	else
		euler.y = std::asin(sinp);

	// roll (z軸回転)
	float siny_cosp = 2.0f * (w * z + x * y);
	float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	euler.z = std::atan2(siny_cosp, cosy_cosp);

	return euler;
}

Math::Quaternion Math::Quaternion::Conjugate() const {
	return Math::Quaternion(-x, -y, -z, w);
}

Math::Vector3 Math::Quaternion::Rotate(const Math::Vector3& vec) const {
	Math::Quaternion qVec(vec.x, vec.y, vec.z, 0); // ベクトルをクォータニオンとして表現
	Math::Quaternion qConj = Conjugate(); // クォータニオンの共役を取る
	Math::Quaternion qResult = *this * qVec * qConj; // クォータニオンによる回転
	return Math::Vector3(qResult.x, qResult.y, qResult.z); // 回転したベクトルを返す
}

float Math::Quaternion::Angle(Math::Quaternion a, Math::Quaternion b) {
	float dot = Math::Quaternion::Dot(a, b);
	dot = std::clamp(dot, -1.0f, 1.0f);
	float angleRadians = 2.0f * std::acos(dot);
	float angleDegrees = angleRadians * kToDegree;

	// 角度は常に正の値
	return angleDegrees;
}

Math::Quaternion Math::Quaternion::operator*(const Math::Quaternion& q2) const {
	Math::Vector3 v1 = Math::Vector3(this->x, this->y, this->z);
	Math::Vector3 v2 = Math::Vector3(q2.x, q2.y, q2.z);

	float dot = Math::Vector3::Dot(v1, v2);
	float newW = (this->w * q2.w) - dot;

	Math::Vector3 cross = CVector3::ZERO;
	cross.x = v1.y * v2.z - v1.z * v2.y;
	cross.y = v1.z * v2.x - v1.x * v2.z;
	cross.z = v1.x * v2.y - v1.y * v2.x;

	Math::Vector3 vector = cross + (v1 * q2.w) + (v2 * this->w);
	return Math::Quaternion(vector.x, vector.y, vector.z, newW);
}

Math::Vector3 Math::Quaternion::operator*(const Math::Vector3& v) const {
	Math::Vector3 qVec{ x, y, z };
	Math::Vector3 t = Math::Vector3::Cross(qVec, v) * 2.0f;
	return v + t * w + Math::Vector3::Cross(qVec, t);
}



#pragma once
#include "Engine/Lib/Math/MyMath.h"

class Quaternion final {
public:


	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {};
	Quaternion(const float& _x, const float& _y, const float& _z, const float& _w) :
		x(_x), y(_y), z(_z), w(_w) {
	};

	~Quaternion() {};


	/// <summary>
	/// 正規化した回転を返す
	/// </summary>
	/// <returns>: 回転</returns>
	Quaternion Normalize() const;

	/// <summary>
	/// 行列を返す
	/// </summary>
	/// <returns>: 回転行列</returns>
	Matrix4x4 MakeMatrix() const;

	Vector3 MakeForward() const;
	Vector3 MakeUp() const;
	Vector3 MakeRight() const;

	/// <summary>
	/// Quaternionからオイラー角
	/// </summary>
	/// <returns>: オイラー角</returns>
	Vector3 ToEulerAngles() const;

	/// <summary>
	/// 共役
	/// </summary>
	/// <returns>: w以外の項をx-1したQuaternion</returns>
	Quaternion Conjugate() const;

	/// <summary>
	/// Vector3との掛け算
	/// </summary>
	/// <param name="vec"></param>
	/// <returns>: Vector3と掛け算をしたあとの回転</returns>
	Vector3 Rotate(const Vector3& vec) const;

public:

	/// <summary>
	/// axisの周りをangle度回転するQuaternionを生成する
	/// </summary>
	/// <param name="angle">: 回転させる角度</param>
	/// <param name="axis">: 回転させる軸</param>
	/// <returns></returns>
	static Quaternion AngleAxis(float angle, const Vector3& axis);


	static Quaternion EulerToQuaternion(const Vector3& euler);

	/// <summary>
	/// Quaternionの逆を返す
	/// </summary>
	/// <param name="rotation"></param>
	/// <returns></returns>
	static Quaternion Inverse(const Quaternion& rotation);

	/// <summary>
	/// from位置からto位置への回転を返す
	/// </summary>
	/// <param name="from">: 現在の位置</param>
	/// <param name="to">: ターゲットの位置</param>
	/// <param name="up">: 回転軸</param>
	/// <returns>: 回転</returns>
	static Quaternion LookAt(const Vector3& from, const Vector3& to, const Vector3& up = Vector3{ 0.0f, 1.0f, 0.0f });

	/// <summary>
	/// from方向からto方向への回転を返す
	/// </summary>
	/// <param name="fromDire">: 元となる方向ベクトル</param>
	/// <param name="toDire">: 向かせたい方向ベクトル</param>
	/// <returns>: to方向へ向く回転</returns>
	static Quaternion FromToRotation(const Vector3& fromDire, const Vector3& toDire);

	/// <summary>
	/// 二つの回転の内積を返す
	/// </summary>
	/// <param name="q1"></param>
	/// <param name="q2"></param>
	/// <returns></returns>
	static float Dot(const Quaternion& q1, const Quaternion& q2);

	static float Angle(Quaternion a, Quaternion b);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="q1"></param>
	/// <param name="q2"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, const float& t);

	static Quaternion RotateTowards(const Quaternion& q1, const Quaternion& q2, float maxDegreesDelta);


	// オイラー角（ラジアン）をクォータニオンに変換
	static Quaternion EulerToQuaternion(float pitch, float yaw, float roll);

	// クォータニオンをオイラー角（ラジアン）に変換
	Vector3 QuaternionToEuler() const;

	static Quaternion ToQuaternion(const Vector4& v);

	/// <summary>
	/// 行列からQuaternionを生成する
	/// </summary>
	/// <param name="m">: 行列</param>
	/// <returns>: Quaternion</returns>
	static Quaternion FromMatrix(const Matrix4x4& m);

	static Quaternion RotateMatrixTo(const Matrix3x3 m);

	/// <summary>
	/// 向きたい方向の回転を求める関数
	/// </summary>
	/// <param name="forward"></param>
	/// <param name="upVector"></param>
	/// <returns></returns>
	static Quaternion LookRotation(const Vector3& forward, const Vector3& upVector = CVector3::UP);

public:

	Quaternion operator*(const Quaternion& q2) const;
	//Vector3 operator*(const Vector3& v);

	Vector3 operator*(const Vector3& v) const;

	operator Vector4() const {
		return Vector4(x, y, z, w);
	}

public:

	float x;
	float y;
	float z;
	float w;

};
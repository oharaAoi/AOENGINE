
float3 ApplyEuler(float3 euler, float3 dire) {
	float cp = cos(euler.x); // pitch
	float sp = sin(euler.x);
	float cy = cos(euler.y); // yaw
	float sy = sin(euler.y);
	float cr = cos(euler.z); // roll
	float sr = sin(euler.z);

    // 回転行列の各軸を構築（XYZ順）
	float3 right = float3(cy * cr, sr, -sy * cr);
	float3 up = float3(-cp * sr, cr, sp * sr);
	float3 forward = float3(sy * cp, -sp, cy * cp);

    // 回転行列を構築
	float3x3 rotMatrix = float3x3(right, up, forward);

    // 上方向ベクトルに回転を適用
	return mul(dire, rotMatrix); // 行ベクトル×行列
}
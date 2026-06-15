
float3 ApplyEuler(float3 euler, float3 dire) {
	
	float cx = cos(euler.x);
	float sx = sin(euler.x);
	float cy = cos(euler.y);
	float sy = sin(euler.y);
	float cz = cos(euler.z);
	float sz = sin(euler.z);

    // 各回転行列（Rx → Ry → Rz）
	float3x3 Rx = float3x3(
        1, 0, 0,
        0, cx, -sx,
        0, sx, cx
    );

	float3x3 Ry = float3x3(
        cy, 0, sy,
        0, 1, 0,
        -sy, 0, cy
    );

	float3x3 Rz = float3x3(
        cz, -sz, 0,
        sz, cz, 0,
        0, 0, 1
    );

    // 最終回転行列（右から掛ける：Rx → Ry → Rz）
	float3x3 rot = mul(Rx, mul(Ry, Rz));

	return mul(dire, rot);
}
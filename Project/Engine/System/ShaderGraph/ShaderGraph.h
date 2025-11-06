#pragma once

/// <summary>
/// ShaderGraphクラス
/// </summary>
class ShaderGraph {
public: // コンストラクタ

	ShaderGraph() = default;
	~ShaderGraph();

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

};


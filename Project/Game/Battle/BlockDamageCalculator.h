#pragma once

/// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

/// <summary>打ち上げたブロックがボスへ与えるダメージの調整値</summary>
struct BlockDamageParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter{

	float perBlock  = 1.0f;   // ブロック1個あたりの加算
	float maxDamage = 100.0f; // 最大ダメージ

	BlockDamageParameter(): CustomParameterSet("Block Damage"){
		SetGroupName("BlockDamage");
		SetName("blockDamageParameter");
		AddParameter("Per Block",perBlock,0.1f,0.0f,1000.0f);
		AddParameter("Max Damage",maxDamage,0.1f,0.0f,1000.0f);
	}

	json ToJson(const std::string& id) const override{
		return AOENGINE::JsonBuilder(id)
			.Add("perBlock",perBlock)
			.Add("maxDamage",maxDamage)
			.Build();
	}

	void FromJson(const json& jsonData) override{
		Convert::fromJson(jsonData,"perBlock",perBlock);
		Convert::fromJson(jsonData,"maxDamage",maxDamage);
	}
};

/// <summary>
/// 打ち上げたブロックがボスに当たった時のダメージを算出するクラス
/// </summary>
class BlockDamageCalculator{
public:

	/// <summary>
	/// 1ヒット分の入力
	/// </summary>
	struct HitContext{
		int blockCount = 0;	// 当たったグループのブロック数
	};

	/// <summary>保存済みの調整値を読み込む</summary>
	void Load(){ parameter_.Load(); }

	/// <summary>1ヒット分のダメージを算出する</summary>
	float Calculate(const HitContext& context) const;

private:

	BlockDamageParameter parameter_;
};

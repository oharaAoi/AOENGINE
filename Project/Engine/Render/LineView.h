#pragma once
#include <cstdint>

namespace AOENGINE {

/// Lineを表示するScene View。ビットマスクとしてGame/Editor/Bothを指定する。
enum class LineView : uint8_t {
	Game = 1,
	Editor = 2,
	Both = 3
};

}

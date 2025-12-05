#pragma once
#include <cstdint>

namespace AOENGINE {

/// <summary>
/// 各Heapのサイズ場所
/// </summary>
class DescriptorSize {
private:

	const uint32_t descriptorSizeSRV_;
	const uint32_t descriptorSizeRTV_;
	const uint32_t descriptorSizeDSV_;

public:

	DescriptorSize(uint32_t _srvSize, uint32_t _rtvSize, uint32_t _dsvSize) :
		descriptorSizeSRV_(_srvSize),
		descriptorSizeRTV_(_rtvSize),
		descriptorSizeDSV_(_dsvSize) {
	}

	uint32_t GetSRV() const { return descriptorSizeSRV_; }
	uint32_t GetRTV() const { return descriptorSizeRTV_; }
	uint32_t GetDSV() const { return descriptorSizeDSV_; }

};

}
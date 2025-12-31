#include "CalcDispatchSize2D.h"

ComputeDispatchSize2D CalcDispatchSize2D(UINT _width, UINT _height, UINT _threadX, UINT _threadY) {
	return ComputeDispatchSize2D(
		(_width + _threadX - 1) / _threadX,
		(_height + _threadY - 1) / _threadY
	);
}

ComputeDispatchSize2D CalcDispatchSize(const D3D12_RESOURCE_DESC& desc, UINT _threadX, UINT _threadY) {
	return CalcDispatchSize2D(
		static_cast<UINT>(desc.Width),
		static_cast<UINT>(desc.Height),
		_threadX,
		_threadY
	);
}

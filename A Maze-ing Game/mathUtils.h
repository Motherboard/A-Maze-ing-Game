#pragma once

namespace mathUtils
{
	template <typename T> 
	int sgn(T x) {
		return  (T(0) < val) - (val < T(0));
	}
}
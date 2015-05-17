#pragma once

namespace mathUtils
{
	template <typename T> 
	int sgn(T x) {
		return  (T(0) < x) - (x < T(0));
	}
}
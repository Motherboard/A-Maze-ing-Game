#pragma once

namespace mathUtils
{
	//return the sign of a variable that supports the < operator.
	template <typename T> 
	int sgn(T x) {
		return  (T(0) < x) - (x < T(0));
	}
}
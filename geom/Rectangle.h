#pragma once
#include "math/IntVec2.h"
using math::IntVec2;

namespace geom {
	struct Rectangle
	{
		Rectangle(IntVec2 position, IntVec2 size);
		IntVec2 Position;
		IntVec2 Size;
	};
}

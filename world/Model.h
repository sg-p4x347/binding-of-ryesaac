#pragma once
#include "pch.h"

#include "ecs/Component.h"

#include "geom/Model.h"

namespace world {
	struct Model :
		public ecs::Component
	{
		Model(shared_ptr<geom::Model> model);
		shared_ptr<geom::Model> ModelPtr;
	};
}

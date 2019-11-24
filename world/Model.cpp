#include "pch.h"

#include "world/Model.h"

namespace world {
	Model::Model(shared_ptr<geom::Model> model) : ModelPtr(model)
	{
	}
}
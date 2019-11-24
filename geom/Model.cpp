#include "pch.h"

#include "Model.h"

namespace geom {
	Model::Model()
	{
	}

	Model::Model(vector<ModelMesh>& meshes) : Meshes(meshes)
	{
	}
}
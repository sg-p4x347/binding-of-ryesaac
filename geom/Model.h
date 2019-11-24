#pragma once
#include "pch.h"

#include "ModelMesh.h"
#include "Material.h"

namespace geom {
	struct Model
	{
		Model();
		Model(vector<ModelMesh>& meshes);
		vector<ModelMesh> Meshes;
		map<string, shared_ptr<Material>> Materials;
	};
}

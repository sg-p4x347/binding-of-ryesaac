#pragma once
#include "pch.h"
#include "ModelMeshVertex.h"
#include "Material.h"
struct ModelMesh
{
	vector<ModelMeshVertex> Vertices;
	vector<uint32_t> Indices;
	shared_ptr<Material> Material;
};


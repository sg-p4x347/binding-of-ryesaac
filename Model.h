#pragma once
#include "pch.h"
#include "ModelMesh.h"
struct Model
{
	Model(vector<ModelMesh> & meshes);
	vector<ModelMesh> Meshes;
};


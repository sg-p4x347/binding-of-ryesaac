#pragma once
#include "pch.h"
#include "Model.h"

class ModelRepository
{
public:
	static ModelRepository& GetInstance();
	static shared_ptr<Model> Get(string modelName);
private:
	ModelRepository();
private:
	static path m_directory;
	map<string, shared_ptr<Model>> m_models;
};


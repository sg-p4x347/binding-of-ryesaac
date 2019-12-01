#pragma once
#include "pch.h"

#include "Model.h"

namespace geom {
	class ModelRepository
	{
	public:
		static ModelRepository& GetInstance();
		static shared_ptr<Model> Get(string modelName);
	private:
		ModelRepository();
	private:
		static const path k_directory;
		map<string, shared_ptr<Model>> m_models;
	};
}

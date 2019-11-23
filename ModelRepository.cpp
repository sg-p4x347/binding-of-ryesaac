#include "pch.h"
#include "ModelRepository.h"
#include "ModelImporter.h"
path ModelRepository::m_directory = "./Assets";

ModelRepository & ModelRepository::GetInstance()
{
	static ModelRepository instance;
	return instance;
}

shared_ptr<Model> ModelRepository::Get(string modelName)
{
	return GetInstance().m_models[modelName];
}

ModelRepository::ModelRepository()
{
	// Import all .fbx files in the assets directory
	for (auto& entry : std::filesystem::directory_iterator(m_directory)) {
		if (
			entry.is_regular_file() 
			&& entry.path().has_extension() 
			&& entry.path().extension() == ".fbx") {
			// Import the fbx file as a Model instance and cache it by filename (without extension)
			m_models[entry.path().stem().string()] = std::make_shared<Model>(ModelImporter::CreateFromFBX(entry.path()));
		}
	}
}

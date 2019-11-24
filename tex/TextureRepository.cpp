#include "pch.h"

#include "TextureRepository.h"

namespace tex {
	path TextureRepository::m_directory = "./Assets";

	TextureRepository& TextureRepository::GetInstance()
	{
		static TextureRepository instance;
		return instance;
	}

	shared_ptr<Bitmap> TextureRepository::Get(string textureName)
	{
		return GetInstance().m_textures[textureName];
	}

	TextureRepository::TextureRepository()
	{
		// Import all .bmp files in the assets directory
		for (auto& entry : std::filesystem::directory_iterator(m_directory)) {
			if (
				entry.is_regular_file()
				&& entry.path().has_extension()
				&& entry.path().extension() == ".bmp") {
				// Import the bmp file as a Bitmap instance and cache it by filename (without extension)
				m_textures.insert(
					std::make_pair(
						entry.path().stem().string(),
						std::make_shared<Bitmap>(Bitmap::FromFile(entry.path()))
					)
				);
			}
		}
	}
}
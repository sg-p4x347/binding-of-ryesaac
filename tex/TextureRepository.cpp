#include "pch.h"

#include "TextureRepository.h"

namespace tex {
	const path TextureRepository::k_directory = "./Assets";

	TextureRepository& TextureRepository::GetInstance()
	{
		static TextureRepository instance;
		return instance;
	}

	shared_ptr<Bitmap> TextureRepository::GetBitmap(string textureName)
	{
		return GetInstance().m_textures[textureName];
	}

	GLuint TextureRepository::GetID(string textureName)
	{
		return GetInstance().m_glTextureIds[textureName];
	}

	TextureRepository::TextureRepository()
	{
		// Import all .bmp files in the assets directory
		for (auto& entry : std::filesystem::directory_iterator(k_directory)) {
			if (
				entry.is_regular_file()
				&& entry.path().has_extension()
				&& entry.path().extension() == ".bmp") {
				// Import the bmp file as a Bitmap instance and cache it by filename (without extension)
				auto bitmap = std::make_shared<Bitmap>(Bitmap::FromFile(entry.path()));
				m_textures.insert(
					std::make_pair(
						entry.path().stem().string(),
						bitmap
					)
				);
				GLuint textureID;

				// Register the texture with OpenGLs
				glEnable(GL_TEXTURE_2D);
				glGenTextures(1, &textureID);
				// "Bind" the newly created texture : all future texture functions will modify this texture
				glBindTexture(GL_TEXTURE_2D, textureID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				// Generate mipmaps
				gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA, bitmap->GetWidth(), bitmap->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, bitmap->GetPixels());
				// Give the image to OpenGL
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap->GetWidth(), bitmap->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->GetPixels());

				m_glTextureIds.insert(
					std::make_pair(
						entry.path().stem().string(),
						textureID
					)
				);
			}
		}
	}
}
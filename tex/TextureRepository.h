#pragma once
#include "pch.h"

#include "Bitmap.h"

#include <GL/glut.h>

namespace tex {
	class TextureRepository
	{
	public:
		static TextureRepository& GetInstance();
		static shared_ptr<Bitmap> GetBitmap(string textureName);
		static GLuint GetID(string textureName);
	private:
		TextureRepository();
	private:
		static const path k_directory;
		map<string, shared_ptr<Bitmap>> m_textures;
		map<string, GLuint> m_glTextureIds;
	};
}

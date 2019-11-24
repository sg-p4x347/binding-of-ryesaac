#pragma once
#include "pch.h"

#include "Bitmap.h"

namespace tex {
	class TextureRepository
	{
	public:
		static TextureRepository& GetInstance();
		static shared_ptr<Bitmap> Get(string textureName);
	private:
		TextureRepository();
	private:
		static const path k_directory;
		map<string, shared_ptr<Bitmap>> m_textures;
	};
}

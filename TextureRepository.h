#pragma once
#include "pch.h"
#include "Bitmap.h"
class TextureRepository
{
public:
	static TextureRepository& GetInstance();
	static shared_ptr<Bitmap> Get(string textureName);
private:
	TextureRepository();
private:
	static path m_directory;
	map<string, shared_ptr<Bitmap>> m_textures;
};


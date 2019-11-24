#pragma once
#include "pch.h"

#include "Pixel.h"

namespace tex {
	class Bitmap {
	public:
		Bitmap(size_t width, size_t height);
		Bitmap(const Bitmap&& other);
		~Bitmap();
		static Bitmap FromFile(path file);
		Pixel GetInstance(int x, int y) const;
		void Set(size_t x, size_t y, Pixel&& pixel);
		void Set(size_t x, size_t y, Pixel& pixel);
		void Set(size_t x, size_t y, size_t width, size_t height, Pixel& pixel);
		void Add(size_t x, size_t y, Pixel& pixel);
		void Add(size_t x, size_t y, size_t width, size_t height, Pixel& pixel);

		Pixel* GetPixels();
		size_t GetWidth();
		size_t GetHeight();
		vector<uint8_t> GetOpaqueBitMask();
	private:
		const size_t m_width;
		const size_t m_height;
		Pixel* m_pixels;
	};
}
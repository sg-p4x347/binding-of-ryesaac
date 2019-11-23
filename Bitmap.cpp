#include "pch.h"
#include "Bitmap.h"
#include <bitset>
Bitmap::Bitmap(size_t width, size_t height) : m_width(width), m_height(height), m_pixels(new Pixel[width * height * sizeof(Pixel)]) {}
Bitmap::Bitmap(const Bitmap&& other) : Bitmap::Bitmap(other.m_width, other.m_height) {
	std::memcpy(m_pixels, other.m_pixels, other.m_height * other.m_height * sizeof(Pixel));
}
Bitmap::~Bitmap()
{
	delete[] m_pixels;
}
Bitmap Bitmap::FromFile(path file)
{
	// Open the file
	ifstream bmp(file, std::ios::binary);

	if (bmp.is_open()) {
		char headerField[2];
		bmp.read((char*)&headerField, sizeof(headerField));
		if (headerField[0] == 'B' && headerField[1] == 'M') {

			bmp.seekg(10); // Data offset
			int dataOffset;
			bmp.read((char*)&dataOffset, sizeof(dataOffset));
			int width;
			int height;
			bmp.seekg(18); // Width
			bmp.read((char*)&width, sizeof(width));
			bmp.read((char*)&height, sizeof(height));
			Bitmap bitmap(width, height);
			bmp.seekg(28); // bits per pixel (must be 32)
			unsigned short bitsPerPixel;
			bmp.read((char*)&bitsPerPixel, sizeof(bitsPerPixel));
			if (bitsPerPixel == sizeof(Pixel) * 8) {
				size_t pixelCount = width * height;
				bmp.seekg(dataOffset);
				for (size_t pixelIndex = 0; pixelIndex < pixelCount; pixelIndex++) {
					uint8_t a, r, g, b;
					bmp.read((char*)&b, sizeof(b));
					bmp.read((char*)&g, sizeof(g));
					bmp.read((char*)&r, sizeof(r));
					bmp.read((char*)&a, sizeof(a));
					bitmap.m_pixels[pixelIndex] = Pixel(r, g, b, a);
				}

			}


			return bitmap;
		}

	}
	return Bitmap(0, 0);
}
Pixel Bitmap::GetInstance(int x, int y) const
{
	if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
		return m_pixels[y * m_width + x];
	}
	else {
		return Pixel(0, 0, 0, 0);
	}

}
void Bitmap::Set(size_t x, size_t y, Pixel&& pixel)
{
	m_pixels[y * m_width + x] = pixel;
}
void Bitmap::Set(size_t x, size_t y, Pixel& pixel)
{
	Set(x, y, std::move(pixel));
}
void Bitmap::Set(size_t x, size_t y, size_t width, size_t height, Pixel& pixel)
{
	for (size_t rectX = x; rectX < x + width; rectX++) {
		for (size_t rectY = y; rectY < y + height; rectY++) {
			Set(rectX, rectY, std::move(pixel));
		}
	}
}
void Bitmap::Add(size_t x, size_t y, Pixel& pixel)
{
	Set(x, y, GetInstance(x, y) + pixel);
}
void Bitmap::Add(size_t x, size_t y, size_t width, size_t height, Pixel& pixel)
{
	for (size_t rectX = x; rectX < x + width; rectX++) {
		for (size_t rectY = y; rectY < y + height; rectY++) {
			Add(rectX, rectY, pixel);
		}
	}
}
Pixel* Bitmap::GetPixels()
{
	return m_pixels;
}
size_t Bitmap::GetWidth()
{
	return m_width;
}
size_t Bitmap::GetHeight()
{
	return m_height;
}
vector<uint8_t> Bitmap::GetOpaqueBitMask()
{
	size_t rowSize = std::ceil((double)m_width / 8.0) * sizeof(uint8_t);
	vector<uint8_t> bits(rowSize * m_height,0);
	for (size_t y = 0; y < m_height; y++) {
		for (size_t x = 0; x < m_width; x++) {
			Pixel pixel = GetInstance(x, y);
			if (pixel.A == 255) {
				size_t byteIndex = x / 8;
				uint8_t & byte = bits[y * rowSize + byteIndex];
				byte = byte + (128 >> (x - byteIndex * 8));
			}
		}
	}
	return bits;
}

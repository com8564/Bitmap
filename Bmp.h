#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>

class BMP {
public:
	unsigned char* LoadBMP24(const char* filename, int& height, int& width);
	bool SaveBMP24(const char* filename, int height, int width, int bpp, unsigned char* pBmpImage);
	bool Calculate_Histogram(unsigned char* pBmpImage, const char* filename, int height, int width, int bpp);
	unsigned char* Flip_Right_Left(unsigned char* input, int height, int width);
	unsigned char* Brightness(unsigned char* input, int height, int width, int value);
	unsigned char* Resize(unsigned char* input, int height, int width, int bpp, int& dHeight, int& dWidth, int ratio);
};
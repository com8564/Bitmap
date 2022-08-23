#define _CRT_SECURE_NO_WARNINGS
#include "Bmp.h"

//load BitMap File
unsigned char* BMP::LoadBMP24(const char* filename, int& height, int& width) {
    BITMAPFILEHEADER bfh; //bitmap fileheader
    BITMAPINFOHEADER bih; //bitmap infoheader

    unsigned char* pBmpImage;
    memset(&bfh, 0, sizeof(BITMAPFILEHEADER)); //initialization
    memset(&bih, 0, sizeof(BITMAPINFOHEADER));

    FILE* fp = nullptr;
    if (fopen_s(&fp, filename, "rb") != 0) {
        printf("no file"); //No Open File
        return NULL;
    }

    fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
    if (bfh.bfType != 0x4d42) { //File is opened, But Not Bitmap File
        printf("no bitmap file");
        fclose(fp);
        return NULL;
    }
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
    if (bih.biBitCount != 24) { //File is opened, But Not 24bit/pixel File
        printf("no 24bitmap/pixel file");
        fclose(fp);
        return NULL;
    }

    //memory alloc
    int imgDataSize = bih.biSizeImage;
    height = bih.biHeight;
    width = bih.biWidth;
    pBmpImage = (unsigned char*)malloc(imgDataSize);
    if (pBmpImage == NULL) {
        fclose(fp);
        return NULL;
    }

    unsigned char trash[3];
    int count_padding = (4 - (bih.biWidth % 4)) % 4;

    if (count_padding == 0) {
        if (bih.biHeight > 0) {
            for (int Y = bih.biHeight - 1; Y >= 0; Y--) {
                fread(pBmpImage + bih.biWidth * 3 * Y, bih.biWidth, 3, fp);
            }
        }
        else {
            fread(pBmpImage, imgDataSize, 1, fp);
        }
    }

    else {
        if (bih.biHeight > 0) {
            for (int Y = bih.biHeight - 1; Y >= 0; Y--) {
                fread(pBmpImage + bih.biWidth * 3 * Y, bih.biWidth, 3, fp);
                fread(trash, count_padding, 1, fp);
            }
        }
        else {
            for (int y = 0; y < bih.biHeight; y++) {
                fread(pBmpImage + bih.biWidth * 3 * y, bih.biWidth, 3, fp);
                fread(trash, count_padding, 3, fp);
            }
        }
    }
    fclose(fp);

    return pBmpImage;
}

//Save Bitmap File
bool BMP::SaveBMP24(const char* filename, int height, int width, int bpp, unsigned char* pBmpImage) {
    if (bpp != 1 && bpp != 3 && bpp != 4) {
        return false;
    }

    FILE* fp = NULL;
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
    memset(&bih, 0, sizeof(BITMAPINFOHEADER));

    int paddingSize = width % 4;

    bfh.bfType = 0x4d42;
    bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (width * height * bpp + height * paddingSize);
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if (bpp == 1)
    {
        bfh.bfSize += (256 * 4);
        bfh.bfOffBits += (256 * 4);
    }
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = -height;
    bih.biPlanes = 1;
    bih.biBitCount = bpp * 8;
    bih.biSizeImage = (width * height * bpp + height * paddingSize);
    bih.biXPelsPerMeter = 2834;
    bih.biYPelsPerMeter = 2834;

    if (fopen_s(&fp, filename, "wb") != 0) {
        return false;
    }
    fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);

    if (paddingSize == 0) {
        fwrite(pBmpImage, width * height * bpp, 1, fp);
    }
    else {
        unsigned char arrPad[3] = { 0,0,0 };
        int widthInBytes = width * 3;

        for (int i = 0; i < height; i++) {
            fwrite(pBmpImage + (i * widthInBytes), 3, bih.biWidth, fp);
            fwrite(arrPad, paddingSize, 1, fp);
        }
    }
    fclose(fp);
    return true;
}

//Calculate Bitmap File
bool BMP::Calculate_Histogram(unsigned char* pBmpImage, const char* filename, int height, int width, int bpp) {
    int red[256] = { 0, }, green[256] = { 0, }, blue[256] = { 0, };

    for (int i = 0; i < width * height * bpp; i += 3)
    {
        red[pBmpImage[i + 2]] += 1;
        green[pBmpImage[i + 1]] += 1;
        blue[pBmpImage[i]] += 1;
    }

    std::ofstream histogram_file(filename, std::ios::out);
    if (!(histogram_file.is_open())) {
        printf("Histogram file is not opened");
        return false;
    }
    //header
    for (int i = 0; i < 256; i++) {
        histogram_file << i << ",";
    }

    //red
    histogram_file << "\n";
    for (int i = 0; i < 256; i++) {
        histogram_file << (int)red[i] << ",";
    }

    //green
    histogram_file << "\n";
    for (int i = 0; i < 256; i++) {
        histogram_file << (int)green[i] << ",";
    }

    //blue
    histogram_file << "\n";
    for (int i = 0; i < 256; i++) {
        histogram_file << (int)blue[i] << ",";
    }

    return true;
}

unsigned char* BMP::Flip_Right_Left(unsigned char* input, int height, int width) {
    unsigned char* dest_bmp = new unsigned char[height * width * 3];

    for (int Y = 0; Y < height; Y++) {
        for (int X = 0; X < width * 3; X += 3) {
            int in = (width * 3 * Y) + ((width * 3) - 1 - X);
            int out = (width * 3 * Y) + X;

            dest_bmp[out] = input[in - 2];
            dest_bmp[out + 1] = input[in - 1];
            dest_bmp[out + 2] = input[in];
        }
    }

    return dest_bmp;
}

//Adjust Brightness
unsigned char* BMP::Brightness(unsigned char* input, int height, int width, int value) {
    unsigned char* dest_bmp = new unsigned char[height * width * 3];
    double ratio = value * 0.01 + 1;

    for (int Y = 0; Y < height; Y++) {
        for (int X = 0; X < width * 3; X++) {
            int pos = (width * 3 * Y) + X;
            int change = input[pos] * ratio;

            if (change > 255) change = 255;
            if (change < 0) change = 0;

            dest_bmp[pos] = change;
        }
    }

    return dest_bmp;
}

//Only 2x magnification
unsigned char* BMP::Resize(unsigned char* input, int height, int width, int bpp, int& dHeight, int& dWidth, int ratio) {

    unsigned char* dest_bmp = new unsigned char[(height * ratio) * (width * ratio) * bpp];

    for (int Y = 0; Y < height; Y++) {
        for (int X = 0; X < width * bpp; X += bpp) {
            int in_pos = (width * bpp * Y) + X;
            int out_pos = (width * bpp * Y * ratio * ratio) + (X * ratio);

            for (int i = 0; i < ratio * 3; i+=3) {
                dest_bmp[out_pos + i] = input[in_pos];
                dest_bmp[out_pos + i + 1] = input[in_pos + 1];
                dest_bmp[out_pos + i + 2] = input[in_pos + 2];
            }

        }
        for (int row = 0; row < ratio * ratio; row++) {
            memcpy(&dest_bmp[(width * 3 * (Y * ratio * ratio)) + ((width * 3 * ratio) * row)]    // ((position) + (width)) length == nextline start position
                , &dest_bmp[width * 3 * (Y * ratio * ratio)]    // position
                , sizeof(char) * width * 3 * ratio);
        }
    }

    
   /* for (int Y = 0; Y < height * ratio; Y += ratio) {
        for (int X = 0; X < width * ratio * bpp; X += bpp) {
            for (int i = 1; i < ratio; i++) {
                dest_bmp[(width * ratio * bpp * (Y + i)) + X] = dest_bmp[(width * ratio * bpp * Y) + X];
                dest_bmp[(width * ratio * bpp * (Y + i)) + X + 1] = dest_bmp[(width * ratio * bpp * Y) + X + 1];
                dest_bmp[(width * ratio * bpp * (Y + i)) + X + 2] = dest_bmp[(width * ratio * bpp * Y) + X + 2];
            }
        }
    }*/

    dHeight = height * ratio;
    dWidth = width * ratio;

    return dest_bmp;
}

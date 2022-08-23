#include "Bmp.h"

int main() {
    BMP bmp;
    int height, width;
    char inputFile[100];
    printf("Input File name : ");
    scanf_s("%s", inputFile, sizeof(inputFile));

    unsigned char* bitmap = bmp.LoadBMP24(inputFile, height, width);
    bmp.Calculate_Histogram(bitmap,"histogram.csv", height, width, 3);

    //Flip
    unsigned char* output1 = bmp.Flip_Right_Left(bitmap, height, width);
    if (output1 == nullptr) {
        printf("Flip Error");
    }
    bmp.SaveBMP24("output1.bmp", height, width, 3, output1);
    delete[] output1;

    int brightRatio = 0;
    printf("Bright Ratio : ");
    scanf_s("%d", &brightRatio);
    unsigned char* output2 = bmp.Brightness(bitmap, height, width, brightRatio);
    if (output2 == nullptr) {
        printf("Bright Error");
    }
    bmp.SaveBMP24("output2.bmp", height, width, 3, output2);
    delete[] output2;

    int resize_scale = 0;
    printf("Input Resize Scale : ");
    scanf_s("%d", &resize_scale);
    int dHeight, dWidth;
    unsigned char* output3 = bmp.Resize(bitmap, height, width, 3, dHeight , dWidth, resize_scale);
    if (output3 == nullptr) {
        printf("Resize Error");
    }
    bmp.SaveBMP24("output3.bmp", dHeight, dWidth, 3, output3);
    delete[] output3;

}
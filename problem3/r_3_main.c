#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "r_3_Bitmap.h"


// 缩放位图图像
void scaleBitmap(const char *inputFile, int scalePercent, const char *outputFile)
{
    FILE *input = fopen(inputFile, "rb");
    if (input == NULL)
    {
        printf("无法打开输入文件：%s\n", inputFile);
        return;
    }

    FILE *output = fopen(outputFile, "wb");
    if (output == NULL)
    {
        printf("无法打开输出文件：%s\n", outputFile);
        fclose(input);
        return;
    }

    // 读取位图文件头
    BitmapFileHeader fileHeader;
    fread(&fileHeader, sizeof(BitmapFileHeader), 1, input);

    // 读取位图信息头
    BitmapInfoHeader infoHeader;
    fread(&infoHeader, sizeof(BitmapInfoHeader), 1, input);

    /* RgbColor colorHeader;
     fread(&colorHeader, sizeof(BitmapInfoHeader), 1, input);*/

    // 计算缩放后的宽度和高度
    int scaleWidth = infoHeader.width * scalePercent / 100;
    int scaleHeight = infoHeader.height * scalePercent / 100;

    // 计算每行像素的字节数
    int rowSize = ((infoHeader.width * infoHeader.bitCount + 31) / 32) * 4;
    int scaleRowSize = ((scaleWidth * infoHeader.bitCount + 31) / 32) * 4;

    // 计算图像数据的大小
    int dataSize = infoHeader.height * rowSize;

    // 分配内存存储图像数据
    unsigned char *imageData = (unsigned char *)malloc(dataSize);
    if (imageData == NULL)
    {
        printf("内存分配失败\n");
        fclose(input);
        fclose(output);
        return;
    }

    // 读取图像数据
    fread(imageData, dataSize, 1, input);

    // 更新位图信息头中的宽度和高度
    infoHeader.width = scaleWidth;
    infoHeader.height = scaleHeight;

    // 计算新的文件大小
    fileHeader.fileSize = fileHeader.dataOffset + scaleHeight * scaleRowSize;
    infoHeader.imageSize = scaleHeight * scaleRowSize;
    unsigned char *imageScaleData = (unsigned char *)malloc(infoHeader.imageSize);

    // 写入位图文件头和位图信息头
    fwrite(&fileHeader, sizeof(BitmapFileHeader), 1, output);
    fwrite(&infoHeader, sizeof(BitmapInfoHeader), 1, output);
    /*fwrite(&colorHeader, sizeof(RgbColor), 1, output);*/

    // 缩放图像数据
    int x, y;
    for (y = 0; y < scaleHeight; y++)
    {
        for (x = 0; x < scaleWidth; x++)
        {
            int srcX = x * 100 / scalePercent;
            int srcY = y * 100 / scalePercent;

            // 源图像中对应的像素位置
            int srcOffset = srcY * rowSize + srcX * (infoHeader.bitCount / 8);

            // 目标图像中对应的像素位置
            int dstOffset = y * scaleRowSize + x * (infoHeader.bitCount / 8);

            // 复制像素数据
            memcpy(&imageScaleData[dstOffset], &imageData[srcOffset], infoHeader.bitCount / 8);
        }
    }

    // 写入缩放后的图像数据
    fwrite(imageScaleData, infoHeader.imageSize, 1, output);

    // 释放内存和关闭文件
    free(imageData);
    fclose(input);
    fclose(output);

    printf("图像缩放完成：%s -> %s\n", inputFile, outputFile);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("使用方法：%s <输入文件> <缩放比例> <输出文件>\n", argv[0]);
        return 1;
    }

    const char *inputFile = argv[1];
    int scalePercent = atoi(argv[2]);
    const char *outputFile = argv[3];

    scaleBitmap(inputFile, scalePercent, outputFile);

    return 0;
}
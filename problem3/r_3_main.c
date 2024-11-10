#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "r_3_Bitmap.h"


// ����λͼͼ��
void scaleBitmap(const char *inputFile, int scalePercent, const char *outputFile)
{
    FILE *input = fopen(inputFile, "rb");
    if (input == NULL)
    {
        printf("�޷��������ļ���%s\n", inputFile);
        return;
    }

    FILE *output = fopen(outputFile, "wb");
    if (output == NULL)
    {
        printf("�޷�������ļ���%s\n", outputFile);
        fclose(input);
        return;
    }

    // ��ȡλͼ�ļ�ͷ
    BitmapFileHeader fileHeader;
    fread(&fileHeader, sizeof(BitmapFileHeader), 1, input);

    // ��ȡλͼ��Ϣͷ
    BitmapInfoHeader infoHeader;
    fread(&infoHeader, sizeof(BitmapInfoHeader), 1, input);

    /* RgbColor colorHeader;
     fread(&colorHeader, sizeof(BitmapInfoHeader), 1, input);*/

    // �������ź�Ŀ�Ⱥ͸߶�
    int scaleWidth = infoHeader.width * scalePercent / 100;
    int scaleHeight = infoHeader.height * scalePercent / 100;

    // ����ÿ�����ص��ֽ���
    int rowSize = ((infoHeader.width * infoHeader.bitCount + 31) / 32) * 4;
    int scaleRowSize = ((scaleWidth * infoHeader.bitCount + 31) / 32) * 4;

    // ����ͼ�����ݵĴ�С
    int dataSize = infoHeader.height * rowSize;

    // �����ڴ�洢ͼ������
    unsigned char *imageData = (unsigned char *)malloc(dataSize);
    if (imageData == NULL)
    {
        printf("�ڴ����ʧ��\n");
        fclose(input);
        fclose(output);
        return;
    }

    // ��ȡͼ������
    fread(imageData, dataSize, 1, input);

    // ����λͼ��Ϣͷ�еĿ�Ⱥ͸߶�
    infoHeader.width = scaleWidth;
    infoHeader.height = scaleHeight;

    // �����µ��ļ���С
    fileHeader.fileSize = fileHeader.dataOffset + scaleHeight * scaleRowSize;
    infoHeader.imageSize = scaleHeight * scaleRowSize;
    unsigned char *imageScaleData = (unsigned char *)malloc(infoHeader.imageSize);

    // д��λͼ�ļ�ͷ��λͼ��Ϣͷ
    fwrite(&fileHeader, sizeof(BitmapFileHeader), 1, output);
    fwrite(&infoHeader, sizeof(BitmapInfoHeader), 1, output);
    /*fwrite(&colorHeader, sizeof(RgbColor), 1, output);*/

    // ����ͼ������
    int x, y;
    for (y = 0; y < scaleHeight; y++)
    {
        for (x = 0; x < scaleWidth; x++)
        {
            int srcX = x * 100 / scalePercent;
            int srcY = y * 100 / scalePercent;

            // Դͼ���ж�Ӧ������λ��
            int srcOffset = srcY * rowSize + srcX * (infoHeader.bitCount / 8);

            // Ŀ��ͼ���ж�Ӧ������λ��
            int dstOffset = y * scaleRowSize + x * (infoHeader.bitCount / 8);

            // ������������
            memcpy(&imageScaleData[dstOffset], &imageData[srcOffset], infoHeader.bitCount / 8);
        }
    }

    // д�����ź��ͼ������
    fwrite(imageScaleData, infoHeader.imageSize, 1, output);

    // �ͷ��ڴ�͹ر��ļ�
    free(imageData);
    fclose(input);
    fclose(output);

    printf("ͼ��������ɣ�%s -> %s\n", inputFile, outputFile);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("ʹ�÷�����%s <�����ļ�> <���ű���> <����ļ�>\n", argv[0]);
        return 1;
    }

    const char *inputFile = argv[1];
    int scalePercent = atoi(argv[2]);
    const char *outputFile = argv[3];

    scaleBitmap(inputFile, scalePercent, outputFile);

    return 0;
}
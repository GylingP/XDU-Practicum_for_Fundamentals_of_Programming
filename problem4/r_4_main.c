#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
void compressFile(const char* inputFile, const char* outputFile);
void decompressFile(const char* inputFile, const char* outputFile);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: program_name file1 -c(-d) file2\n");
        return 1;
    }

    const char* inputFile = argv[1];
    const char* option = argv[2];
    const char* outputFile = argv[3];

    if (strcmp(option, "-c") == 0) {
        compressFile(inputFile, outputFile);
    } else if (strcmp(option, "-d") == 0) {
        decompressFile(inputFile, outputFile);
    } else {
        printf("Invalid option: %s\n", option);
        return 1;
    }

    printf("Operation completed successfully.\n");
    return 0;
}

void compressFile(const char* inputFile, const char* outputFile) {
    FILE* input = fopen(inputFile, "rb");
    if (input == NULL) {
        printf("Error opening input file: %s\n", inputFile);
        exit(1);
    }

    FILE* output = fopen(outputFile, "wb");
    if (output == NULL) {
        printf("Error creating output file: %s\n", outputFile);
        exit(1);
    }

    int currentByte, previousByte, count;

    // 读取第一个字节
    if ((currentByte = fgetc(input)) == EOF) {
        fclose(input);
        fclose(output);
        return;
    }

    count = 1;
    previousByte = currentByte;

    while ((currentByte = fgetc(input)) != EOF) {
        if (currentByte == previousByte && count < 255) {
            // 如果当前字节与前一个字节相同，并且计数小于255，则增加计数
            count++;
        } else {
            // 否则，将前一个字节和计数写入输出文件
            fputc(count, output);
            fputc(previousByte, output);
            count = 1;
            previousByte = currentByte;
        }
    }

    // 写入最后一个字节
    fputc(count, output);
    fputc(previousByte, output);

    fclose(input);
    fclose(output);
}

void decompressFile(const char* inputFile, const char* outputFile) {
    FILE* input = fopen(inputFile, "rb");
    if (input == NULL) {
        printf("Error opening input file: %s\n", inputFile);
        exit(1);
    }

    FILE* output = fopen(outputFile, "wb");
    if (output == NULL) {
        printf("Error creating output file: %s\n", outputFile);
        exit(1);
    }

    int count, byte;

    while ((count = fgetc(input)) != EOF) {
        byte = fgetc(input);
        for (int i = 0; i < count; i++) {
            fputc(byte, output);
        }
    }

    fclose(input);
    fclose(output);
}
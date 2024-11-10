// 位图文件头结构体
#pragma pack(2)
typedef struct
{
    unsigned short fileType;
    unsigned int fileSize;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int dataOffset;
} BitmapFileHeader;

// 位图信息头结构体
typedef struct
{
    unsigned int headerSize;
    long width;
    long height;
    unsigned short planes;
    unsigned short bitCount;
    unsigned int compression;
    unsigned int imageSize;
    long xPixelsPerMeter;
    long yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int colorsImportant;
} BitmapInfoHeader;

/*// RGB颜色结构体
typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char rgbReserved;
} RgbColor;*/

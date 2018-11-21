#include "gba.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void) {
    // TA-TODO: IMPLEMENT
    while (*SCANLINECOUNTER > 160);
    while (*SCANLINECOUNTER < 160);
    vBlankCounter++;

}

static int __qran_seed= 42;
static int qran(void) {
    __qran_seed= 1664525*__qran_seed+1013904223;
    return (__qran_seed>>16) & 0x7FFF;
}

int randint(int min, int max) {
    return (qran()*(max-min)>>15)+min;
}

void setPixel(int x, int y, u16 color) {
    // TA-TODO: IMPLEMENT
    videoBuffer[OFFSET(y, x, WIDTH)] = color;
}

void drawRectDMA(int x, int y, int width, int height, volatile u16 color) {
    // TA-TODO: IMPLEMENT
    for (int row = 0; row < height; row++) {
        DMA[3].src = &color;
        DMA[3].dst = &videoBuffer[OFFSET(row + x, y, 240)];
        DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
    }
    //videoBuffer[y + WIDTH * x] = 0x2ac0;
    //videoBuffer[y + WIDTH * (x + 2)] = 0x2ac0;
    //videoBuffer[(y + 2) + WIDTH * x] = 0x2ac0;
    //videoBuffer[(y + 2) + WIDTH * (x + 2)] = 0x2ac0;
}

void drawFullScreenImageDMA(const u16 *image) {
    // TA-TODO: IMPLEMENT
    DMA[3].src = image;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = 240 * 160 | DMA_SOURCE_INCREMENT |
                DMA_DESTINATION_INCREMENT | DMA_ON;
}

void drawImageDMA(int x, int y, int width, int height,const u16 *image) {
    // TA-TODO: IMPLEMENT
    for (int row = 0; row < height; row++) {
        DMA[3].src = image + OFFSET(row, 0, width);
        DMA[3].dst = videoBuffer + OFFSET(x + row, y, 240);
        DMA[3].cnt = width | DMA_SOURCE_INCREMENT |
                    DMA_DESTINATION_INCREMENT | DMA_ON;
    }
}

void undrawDMA(const u16 *image) {
    drawFullScreenImageDMA(image);
}

void fillScreenDMA(volatile u16 color) {
    // TA-TODO: IMPLEMENT
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = 240*160 | DMA_ON | DMA_SOURCE_FIXED;
}

void drawChar(int col, int row, char ch, u16 color) {
    for(int r = 0; r<8; r++) {
        for(int c=0; c<6; c++) {
            if(fontdata_6x8[OFFSET(r, c, 6) + ch*48]) {
                setPixel(col+c, row+r, color);
            }
        }
    }
}

void drawString(int col, int row, char *str, u16 color) {
    while(*str) {
        drawChar(col, row, *str++, color);
        col += 6;
    }
}

void drawCenteredString(int x, int y, int width, int height, char *str, u16 color) {
    u32 len = 0;
    char *strCpy = str;
    while (*strCpy) {
        len++;
        strCpy++;
    }

    u32 strWidth = 6 * len;
    u32 strHeight = 8;

    int col = x + ((width - strWidth) >> 1);
    int row = y + ((height - strHeight) >> 1);
    drawString(col, row, str, color);
}


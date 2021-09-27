#include "svpng.h"
#include <stdlib.h>
#include <string.h>

#define W 512
#define H 512

unsigned char* bresenham(int start_x, int start_y, int end_x, int end_y) {
    unsigned char* img = (unsigned char*)malloc(W * H * 3 * sizeof(unsigned char));
    memset(img, 255, W * H * 3);
    // 由于图片是从最顶点开始算，因此我们需要将指针移到底部
    unsigned char* bottle = img + (W * H * 3);
    // 首先给起点和终点加上像素
    unsigned char* start_ptr = bottle - (start_y * W + (W - start_x)) * 3;
    *start_ptr++ = (unsigned char)0;
    *start_ptr++ = (unsigned char)0;
    *start_ptr++ = (unsigned char)0;

    unsigned char* end_ptr = bottle - (end_y * W + (W - end_x)) * 3;
    *end_ptr++ = (unsigned char)0;
    *end_ptr++ = (unsigned char)0;
    *end_ptr++ = (unsigned char)0;

    int pos_x = start_x;
    int pos_y = start_y;
    int prev_pos_y = start_y;
    int prev_pos_x = start_x;
    int diff_x = end_x - start_x;
    int diff_y = end_y - start_y;
    float k = diff_x == 0 ? 1 << 16: (diff_y / diff_x);
    int p = 0;
    // 根据斜率的不同进行绘制
    if(k >= 1) {
        p = 2 * diff_x - diff_y;
        while(pos_y < end_y) {
            prev_pos_x = pos_x;
            pos_y += 1;
            if (p > 0) {
                pos_x += 1;
            }
            p = 2 * diff_x - 2 * diff_y * (pos_x - prev_pos_x);
            unsigned char* ptr = bottle - (pos_y * W + (W - pos_x)) * 3;
            *ptr++ = (unsigned char)0;
            *ptr++ = (unsigned char)0;
            *ptr = (unsigned char)0;
        }
    }else if(k >= 0 && k < 1) {
        p = 2 * diff_y - diff_x;
        while(pos_x < end_x) {
            prev_pos_y = pos_y;
            pos_x += 1;
            if (p > 0) {
                pos_y += 1;
            }
            p = 2 * diff_y - 2 * diff_x * (pos_y - prev_pos_y);
            unsigned char* ptr = bottle - (pos_y * W + (W - pos_x)) * 3;
            *ptr++ = (unsigned char)0;
            *ptr++ = (unsigned char)0;
            *ptr = (unsigned char)0;
        }
    }
    return img;
}

int main() {
    int start_x = 128;
    int start_y = 128;
    int end_x = 400;
    int end_y = 500;
    unsigned char* img = bresenham(start_x, start_y, end_x, end_y);
    svpng(fopen("bresenham.png", "wb"), W, H, img, 0);
    return 0;
}
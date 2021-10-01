#include "svpng.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define W 512
#define H 512
#define PI 3.14159265359f

static unsigned char img[W * H * 3];

void setpixel(int x, int y) {
    // 根据坐标获取像素点指针进行绘制
    unsigned char* p = img + (W * (H - y - 1) + x) * 3;;
    p[0] = (unsigned char)0;
    p[1] = (unsigned char)0;
    p[2] = (unsigned char)0;
}

void bresenham(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    // 通过斜率来为err赋值
    int err = (dx > dy ? dx : -dy) / 2;

    // 绘制像素点直到x0和x1重合或者y0和y1重合
    while (setpixel(x0, y0), x0 != x1 || y0 != y1) {
        int e2 = err;
        // 本质上是通过e2判断此时应向宽延展还是应该向高延展
        // 根据决策函数 pk+1 - pk = 2*dy(1 | 0) - 2*dx(1 | 0)
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
}

int main() {
    memset(img, 255, W * H * 3);
    float cx = W * 0.5f - 0.5f, cy = H * 0.5f - 0.5f;
    for (int j = 0; j < 5; j++) {
        float r1 = fminf(W, H) * (j + 0.5f) * 0.085f;
        float r2 = fminf(W, H) * (j + 1.5f) * 0.085f;
        float t = j * PI / 64.0f;
        for (int i = 1; i <= 64; i++, t += 2.0f * PI / 64.0f) {
            float ct = cosf(t), st = sinf(t);
            bresenham((int)(cx + r1 * ct), (int)(cy - r1 * st), (int)(cx + r2 * ct), (int)(cy - r2 * st));
        }
    }
    svpng(fopen("bresenham.png", "wb"), W, H, img, 0);
    return 0;
}
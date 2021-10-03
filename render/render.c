#include "svpng.h"
#include "cvector.h"
#include <stdlib.h>
#include <string.h>

#define W 1024
#define H 1024

unsigned char img[3 * W * H];

typedef struct Vec3f
{
    double x, y, z;
} Vec3f;

typedef struct Vertex {
    int x, y, z;
} Vertex;


void setpixel(int x, int y) {
    // 根据坐标获取像素点指针进行绘制
    unsigned char* p = img + (W * (H - y - 1) + x) * 3;;
    p[0] = (unsigned char)0;
    p[1] = (unsigned char)0;
    p[2] = (unsigned char)0;
}

void draw_line(int x0, int y0, int x1, int y1) {
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
    memset(img, 255, sizeof(img));

    FILE* fp;
    fp = fopen("obj/diablo3_pose.obj", "r");
    char line[256];
    cvector_vector_type(Vec3f) vert = NULL;
    cvector_vector_type(Vertex) face = NULL;
    
    while(fgets(line, sizeof(line), fp)) {
        char* p = line;
        if(*p == 'v') {
            Vec3f vec3f;
            sscanf(p, "v %lf %lf %lf", &vec3f.x, &vec3f.y, &vec3f.z);
            cvector_push_back(vert, vec3f);
        }else if(*p == 'f') {
            Vertex v;
            int trash;
            sscanf(
                p, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
                &v.x, &trash, &trash, &v.y, &trash, &trash, &v.z, &trash, &trash
            );
            v.x -= 1;
            v.y -= 1;
            v.z -= 1;
            cvector_push_back(face, v);
        }
    }
    for(int i = 0; i < cvector_size(face); i++) {
        Vec3f x, y, z;
        x = vert[face[i].x];
        y = vert[face[i].y];
        z = vert[face[i].z];

        int x0 = (x.x + 1.) * W / 2. - 1;
        int y0 = (x.y + 1.) * H / 2. - 1;
        int x1 = (y.x + 1.) * W / 2. - 1;
        int y1 = (y.y + 1.) * H / 2. - 1;
        int x2 = (z.x + 1.) * W / 2. - 1;
        int y2 = (z.y + 1.) * H / 2. - 1;
        // printf("x0: %d, y0: %d\n", x0, y0);
        // printf("x1: %d, y1: %d\n", x1, y1);
        // printf("x2: %d, y2: %d\n", x2, y2);
        draw_line(x0, y0, x1, y1);
        draw_line(x0, y0, x2, y2);
        draw_line(x1, y1, x2, y2);
    }
    svpng(fopen("diablo3_pose.png", "wb"), W, H, img, 0);
}
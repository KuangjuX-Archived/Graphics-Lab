#include <stdlib.h>
#include <string.h>
#include "svpng.h"

#define QUEUE_SIZE 10

#define W 512
#define H 512

typedef struct position {
    int x;
    int y;
}position;

typedef struct line {
    position start;
    position end;
} line;

typedef struct frame {
    int sides;
    line* lines;
}frame;

typedef struct queue {
    position queue[QUEUE_SIZE];
    int size;
}queue;

void push(queue* q, position p) {
    q->queue[q->size] = p;
    q->size += 1;
}

int pop(queue* q, line* l) {
    if(q->size == 2) {
        l->start = q->queue[0];
        l->end = q->queue[1];
        q->size -= 2;
        return 1;
    }else {
        return 0;
    }
}

unsigned char img[W * H * 3];

void draw_line(int start_x, int start_y, int end_x, int end_y) {  
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
    
}


void draw_frame(int start_x, int start_y, int end_x, int end_y) {
    draw_line(start_x, start_y, end_x, start_y);
    draw_line(start_x, start_y, start_x, end_y);
    draw_line(end_x, start_y, end_x, end_y);
    draw_line(start_x, end_y, end_x, end_y);
}

void sutherland_hodgman(frame f, int start_x, int start_y, int end_x, int end_y) {
    queue q;
    q.size = 0;
    for(int i = 0; i < 4; i++) {
        if(i == 0) {
            // 左裁剪
            for(int j = 0; j < f.sides; j++) {
                line l = *(f.lines + j);
                if(l.start.x < start_x && l.end.x < start_x) {
                    // 从外部到外部，不输出任何值
                    continue;
                }else if(l.start.x > start_x && l.end.x > start_x) {
                    // 从内部到内部，输出第二个点
                    position point;
                    point.x = l.end.x;
                    point.y = l.end.y;
                    push(&q, point);
                }else if(l.start.x < start_x && l.end.x > start_x) {
                    // 从外部到内部，输出交点和第二个点
                    position point1, point2;
                    // 计算交点
                    point1.x = start_x;
                    point1.y = l.start.x + ((l.end.y - l.start.y) / (l.end.x - l.start.x)) * (start_x - l.start.x);
                    point2.x = l.end.x;
                    point2.y = l.end.y;
                    push(&q, point1);
                    push(&q, point2);
                }else if(l.start.x > start_x && l.end.x < start_x) {
                    // 从内部到外部，输出交点
                    position point;
                    point.x = start_x;
                    point.y = l.end.x + ((l.start.y - l.end.y) / (l.start.x - l.end.x)) * (start_x - l.end.x);
                    push(&q, point);
                }
            }
        }else if(i == 1) {
            // 右裁剪
        }else if(i == 2) {
            // 下裁剪
        }else if(i == 3) {
            // 上裁剪
        }
    }
}

int main() {
    memset(img, 255, sizeof(img));
    int start_x = 100;
    int start_y = 100;
    int end_x = 300;
    int end_y = 300;
    // 绘制裁减窗口
    draw_frame(start_x, start_y, end_x, end_y);

    // 绘制被裁剪图形
    int a_x = 200;
    int a_y = 200;
    int b_x = 400;
    int b_y = 300;
    int c_x = 400;
    int c_y = 200;
    draw_line(a_x, a_y, b_x, b_y);
    draw_line(a_x, a_y, c_x, c_y);
    draw_line(c_x, c_y, b_x, b_y);

    frame f;
    f.sides = 3;
    f.lines = (line*)malloc(sizeof(line) * 3);
    line* ptr = f.lines;
    (*ptr).start.x = a_x;
    (*ptr).start.y = a_y;
    (*ptr).end.x = b_x;
    (*ptr).end.y = b_y;

    ptr += 1;
    (*ptr).start.x = a_x;
    (*ptr).start.y = a_y;
    (*ptr).end.x = c_x;
    (*ptr).end.y = c_y;

    ptr += 1;
    (*ptr).start.x = c_x;
    (*ptr).start.y = c_y;
    (*ptr).end.x = b_x;
    (*ptr).end.y = b_y;

    sutherland_hodgman(f, start_x, start_y, end_x, end_y);

    svpng(fopen("sutherland.png", "wb"), W, H, img, 0);
}
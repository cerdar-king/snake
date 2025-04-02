#include <stdio.h>
#include <assert.h>

#define WIDTH 10
#define HEIGHT 10

typedef struct {
    int x;
    int y;
    int show_like;
} ROLE;

char ImageF[HEIGHT * WIDTH];

void ImageBoard(ROLE point) {
    char a = ' ';

    if(point.show_like == 1) // 假设 SNAKE_HEAD 是 1
        a = '@';
    else if(point.show_like == 2) // 假设 SNAKE_TAIL 是 2
        a = 'o';
    else if(point.show_like == 3) // 假设 WALL 是 3
        a = '#';
    else if(point.show_like == 4) // 假设 FLOWER 是 4
        a = 'F';
    else
        a = ' ';

    ImageF[point.y * WIDTH + point.x] = a;
}

void setup() {
    for(int i = 0; i < HEIGHT * WIDTH; i++) {
        ImageF[i] = ' ';
    }
}

void test_ImageBoard_SnakeHead() {
    setup();
    ROLE point = {5, 5, 1}; // SNAKE_HEAD
    ImageBoard(point);
    assert(ImageF[5 * WIDTH + 5] == '@');
}

void test_ImageBoard_SnakeTail() {
    setup();
    ROLE point = {5, 5, 2}; // SNAKE_TAIL
    ImageBoard(point);
    assert(ImageF[5 * WIDTH + 5] == 'o');
}

void test_ImageBoard_Wall() {
    setup();
    ROLE point = {5, 5, 3}; // WALL
    ImageBoard(point);
    assert(ImageF[5 * WIDTH + 5] == '#');
}

void test_ImageBoard_Flower() {
    setup();
    ROLE point = {5, 5, 4}; // FLOWER
    ImageBoard(point);
    assert(ImageF[5 * WIDTH + 5] == 'F');
}

void test_ImageBoard_Other() {
    setup();
    ROLE point = {5, 5, 5}; // 其他值
    ImageBoard(point);
    assert(ImageF[5 * WIDTH + 5] == ' ');
}

int main() {
    test_ImageBoard_SnakeHead();
    test_ImageBoard_SnakeTail();
    test_ImageBoard_Wall();
    test_ImageBoard_Flower();
    test_ImageBoard_Other();
    printf("All tests passed!\n");
    return 0;
}
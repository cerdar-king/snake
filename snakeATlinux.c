#include <stdio.h>   // 标准输入输出函数，如 printf、fopen、fread 等
#include <string.h>  // 字符串操作函数，如 strchr、strcmp 等
#include <stdlib.h>  // 通用工具函数，如 rand、srand、exit 等
#include <ncurses.h> // ncurses 库函数，用于终端界面绘制
#include <time.h>    // 时间相关函数，如 time、nanosleep 等
#include <stdarg.h>  // 可变参数处理函数，如 va_list、va_start、va_end 等

int WIDTH = 40;           // 宽度
int HEIGHT = 20;          // 高度
int BACK_G = COLOR_WHITE; // 背景颜色
int SPEED = 150;          // 初生后的初始速度

int x, y, fruitX, fruitY, score; //  x和y是蛇的头，fruitX和fruitY是水果，score是游戏的分数
int tailX[100], tailY[100];      //  蛇的尾巴 x和y坐标
int nTail;                       //  蛇的长度
enum eDirection
{
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
}; //  枚举方向
enum eDirection dir; //  方向
enum eRole
{
    SNAKE = 1,
    WALL,
    FLOWER,
    SCORE,
}; //  枚举角色颜色
enum eRole color; //  角色颜色

// 定义一个函数来替换 usleep
void sleep_ms(int milliseconds)
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

// 彩色画笔函数，用于在指定位置以指定颜色绘制文本
// 参数 color_pair: 颜色对编号，用于指定文本的颜色
// 参数 x, y: 文本绘制的坐标位置，x 为行号，y 为列号
// 参数 fmt: 格式字符串，描述文本的格式
// ...: 可变参数列表，包含格式字符串中的具体值
void DrawColorMvprintw(int color_pair, int x, int y, const char *fmt, ...)
{
    // 初始化可变参数列表
    va_list args;
    va_start(args, fmt);

    // 设置文本颜色
    attron(COLOR_PAIR(color_pair));

    // 缓存区用于存储格式化后的文本
    char buffer[256];

    // 根据格式字符串和可变参数生成实际文本
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    // 在指定位置绘制文本
    mvprintw(x, y, "%s", buffer);

    // 移除文本颜色设置，恢复默认颜色
    attroff(COLOR_PAIR(color_pair));

    // 解除可变参数列表的初始化
    va_end(args);
}

// 从配置文件读取宽度、高度和背景色
/*void read_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening config file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // 去掉换行符
        line[strcspn(line, "\n")] = 0;

        // 查找等号的位置
        char *delimiter = strchr(line, '=');
        if (delimiter) {
            *delimiter = '\0'; // 将等号替换为字符串结束符
            char *key = line;
            char *value = delimiter + 1;

            // 根据键名设置全局变量
            if (strcmp(key, "WIDTH") == 0) {
                WIDTH = atoi(value); // 转换为整数
            } else if (strcmp(key, "HEIGHT") == 0) {
                HEIGHT = atoi(value); // 转换为整数
            } else if (strcmp(key, "BACK_G") == 0) {
                if (strcmp(value, "COLOR_BLACK") == 0) {
                    BACK_G = COLOR_BLACK;
                } else if (strcmp(value, "COLOR_RED") == 0) {
                    BACK_G = COLOR_RED;
                } else if (strcmp(value, "COLOR_WALL") == 0) {
                    BACK_G = COLOR_WALL;
                } else if (strcmp(value, "COLOR_YELLOW") == 0) {
                    BACK_G = COLOR_YELLOW;
                } else if (strcmp(value, "COLOR_BLUE") == 0) {
                    BACK_G = COLOR_BLUE;
                } else if (strcmp(value, "COLOR_WHITE") == 0) {
                    BACK_G = COLOR_WHITE;
                }
            }
        }
    }

    fclose(file);
}*/
/*void read_config(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file) {
        fscanf(file,"%d %d %d",&WIDTH,&HEIGHT,&BACK_G);
        fclose(file);
    }else{
        printf("Error opening config file\n");
    }
    return;
}*/
void read_config(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file)
    {
        size_t len = fread(&WIDTH, sizeof(int), 1, file);
        len = fread(&HEIGHT, sizeof(int), 1, file);
        len = fread(&BACK_G, sizeof(int), 1, file);
        fclose(file);
    }
    else
    {
        printf("Error opening config file\n");
    }
    return;
}

// 生成水果
void generateFruit()
{
    srand(time(NULL));
    int validPosition = 0;
    while (!validPosition)
    {
        fruitX = rand() % (WIDTH - 1);
        fruitY = rand() % (HEIGHT - 1);
        validPosition = 1;
        for (int i = 0; i < nTail; i++)
        {
            if (tailX[i] == fruitX && tailY[i] == fruitY)
            {
                validPosition = 0;
                break;
            }
        }
    }
}

void Setup()
{ //  游戏初始化

    // 初始化游戏内容
    dir = STOP;      //  初始方向为停止
    x = WIDTH / 2;   //  蛇头的初始位置
    y = HEIGHT / 2;  //  蛇头的初始位置
    nTail = 0;       //  初始化蛇的长度
    generateFruit(); //  生成水果
    score = 0;       //  初始化分数

    // 初始化屏幕
    initscr(); //  初始化屏幕

    start_color();        //  开启颜色
    use_default_colors(); //  使用默认颜色
    clear();              //  清屏
    curs_set(0);          //  隐藏光标
    noecho();             //  输入不显示在屏幕上

    // 初始化游戏输入
    cbreak();              //  禁用行缓冲
    nodelay(stdscr, TRUE); // 设置非阻塞输入模式

    // 根据不同的背景颜色，初始化角色颜色对
    switch (BACK_G)
    {
    case COLOR_BLACK:
        init_pair(SNAKE, COLOR_YELLOW, BACK_G);
        init_pair(WALL, COLOR_GREEN, BACK_G);
        init_pair(FLOWER, COLOR_RED, BACK_G);
        init_pair(SCORE, COLOR_WHITE, BACK_G);
        break;
    case COLOR_RED:
        init_pair(SNAKE, COLOR_YELLOW, BACK_G);
        init_pair(WALL, COLOR_BLACK, BACK_G);
        init_pair(FLOWER, COLOR_BLUE, BACK_G);
        init_pair(SCORE, COLOR_WHITE, BACK_G);
        break;
    case COLOR_GREEN:
        init_pair(SNAKE, COLOR_YELLOW, BACK_G);
        init_pair(WALL, COLOR_BLACK, BACK_G);
        init_pair(FLOWER, COLOR_RED, BACK_G);
        init_pair(SCORE, COLOR_WHITE, BACK_G);
        break;
    case COLOR_YELLOW:
        init_pair(SNAKE, COLOR_GREEN, BACK_G);
        init_pair(WALL, COLOR_BLACK, BACK_G);
        init_pair(FLOWER, COLOR_RED, BACK_G);
        init_pair(SCORE, COLOR_WHITE, BACK_G);
        break;
    case COLOR_BLUE:
        init_pair(SNAKE, COLOR_YELLOW, BACK_G);
        init_pair(WALL, COLOR_BLACK, BACK_G);
        init_pair(FLOWER, COLOR_RED, BACK_G);
        init_pair(SCORE, COLOR_WHITE, BACK_G);
        break;
    case COLOR_WHITE:
        init_pair(SNAKE, COLOR_YELLOW, BACK_G);
        init_pair(WALL, COLOR_BLACK, BACK_G);
        init_pair(FLOWER, COLOR_RED, BACK_G);
        init_pair(SCORE, COLOR_BLUE, BACK_G);
        break;
    default:
        perror("Error color configed in data.bin");
        endwin();
        exit(1);
    }
}

void Draw()
{                                           //  画图
    clear();                                //  清屏
    for (int i = 0; i < WIDTH + 2; i++)     //  画上边框
        DrawColorMvprintw(WALL, 0, i, "#"); //  移动光标到(0, i)位置，打印#
    for (int i = 0; i < HEIGHT + 1; i++)
    { //  画左右边框
        for (int j = 0; j < WIDTH + 1; j++)
        {
            if (j == 0)
                DrawColorMvprintw(WALL, i + 1, j, "#");
            if (i == y && j == x)
                DrawColorMvprintw(SNAKE, i + 1, j + 1, "@");
            else if (i == fruitY && j == fruitX)
                DrawColorMvprintw(FLOWER, i + 1, j + 1, "F");
            else
            {
                int print = 0;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        DrawColorMvprintw(SNAKE, i + 1, j + 1, "o");
                        print = 1;
                    }
                }
                if (!print)
                    DrawColorMvprintw(SNAKE, i + 1, j + 1, " ");
            }
            if (j == WIDTH)
                DrawColorMvprintw(WALL, i + 1, j + 1, "#");
        }
    }
    for (int i = 0; i < WIDTH + 2; i++) //  画下边框
        DrawColorMvprintw(WALL, HEIGHT + 1, i, "#");
    DrawColorMvprintw(SCORE, HEIGHT + 2, 0, "Score: %d", score); //  打印分数
    refresh();                                                   //  刷新屏幕
}

void Input()
{ //  输入
    int c;
    while ((c = getch()) != ERR) // 循环读取输入，清空缓冲区
    {
        switch (c)
        {
        case 'a':
            if (dir != RIGHT)
                dir = LEFT; // 防止蛇反向移动
            break;
        case 'd':
            if (dir != LEFT)
                dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN)
                dir = UP;
            break;
        case 's':
            if (dir != UP)
                dir = DOWN;
            break;
        case 'x':
            endwin();
            exit(0);
        }
    }
}

void Logic()
{                         //  逻辑
    int prevX = tailX[0]; //  记录蛇尾的位置
    int prevY = tailY[0]; //  记录蛇尾的位置
    int prev2X, prev2Y;   //  记录蛇尾的位置
    tailX[0] = x;         //  更新蛇尾的位置
    tailY[0] = y;         //  更新蛇尾的位置
    //  更新蛇尾的位置
    for (int i = 1; i < nTail; i++)
    {                      //  更新蛇的位置
        prev2X = tailX[i]; //  记录蛇尾的位置
        prev2Y = tailY[i]; //  记录蛇尾的位置
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // 根据方向更新蛇头的位置
    switch (dir)
    {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    // 如果蛇头碰到边界，从另一边出来
    if (x >= WIDTH)
        x = 0;
    else if (x < 0)
        x = WIDTH - 1;
    if (y >= HEIGHT)
        y = 0;
    else if (y < 0)
        y = HEIGHT - 1;

    // 如果蛇头碰到蛇身，游戏结束
    for (int i = 0; i < nTail; i++)
    {
        if (tailX[i] == x && tailY[i] == y)
        {
            clear();
            DrawColorMvprintw(SNAKE, HEIGHT / 2, (WIDTH - 10) / 2, "Game Over!");
            DrawColorMvprintw(SCORE, HEIGHT / 2 + 1, (WIDTH - 10) / 2, "Score: %d", score);
            refresh();
            DrawColorMvprintw(FLOWER, HEIGHT / 2 + 2, (WIDTH - 10) / 2, "Press Enter to exit");
            // sleep_ms(2000); // 显示2秒
            timeout(-1); // 等待用户输入
            refresh();
            while (getch() != '\n')
                ; // 等待用户按下回车键
            endwin();
            exit(0); //  如果蛇头碰到蛇身，游戏结束
            break;
        }
    }

    // 如果蛇头碰到水果，增加分数，并生成新的水果位置
    if (x == fruitX && y == fruitY)
    {
        score += 10;
        // 确保水果的位置不与蛇的位置重叠
        generateFruit();
        nTail++;
    }
}

int main()
{
    read_config("data.bin");

    Setup(); // 初始化
    while (1)
    {            // 游戏循环
        Draw();  // 画图
        Input(); // 输入
        Logic(); // 逻辑

        // 根据得分动态调整游戏速度
        int speed = SPEED - score / 10;
        if (speed < 30)
            speed = 30;
        sleep_ms(speed); // 等待
    }
    endwin(); // 结束
    return 0;
}
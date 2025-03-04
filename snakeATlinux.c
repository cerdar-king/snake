#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <stdarg.h>

#define WIDTH 20
#define HEIGHT 20

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
enum eColor
{
    GREEN = 1,
    YELLOW,
    RED,
    BLUE
}; //  枚举颜色
enum eColor color; //  颜色

// 定义一个函数来替换 usleep
void sleep_ms(int milliseconds)
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

/*void DrawColorMvprintw(int color_pair, int x, int y, const char *fmt, ...) { //彩色画笔
    va_list args;
    va_start(args, fmt);
    attron(COLOR_PAIR(color_pair));
    mvprintw(x, y, fmt, args);
    attroff(COLOR_PAIR(color_pair));
    va_end(args);
}*/

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

void Setup()
{                                   //  初始化
    dir = STOP;                     //  初始方向为停止
    x = WIDTH / 2;                  //  蛇头的初始位置
    y = HEIGHT / 2;                 //  蛇头的初始位置
    srand(time(NULL));              //  随机数种子
    fruitX = rand() % (WIDTH - 1);  //  随机生成水果的位置
    fruitY = rand() % (HEIGHT - 1); //  随机生成水果的位置
    score = 0;                      //  初始化分数
    initscr();                      //  初始化屏幕
    start_color();                  //  开启颜色
    clear();                        //  清屏
    noecho();                       //  输入不显示在屏幕上
    cbreak();                       //  禁用行缓冲
    timeout(100);                   //  设置getch()的等待时间
    curs_set(0);                    //  隐藏光标
    nTail = 0;                      //  初始化蛇的长度

    // 初始化颜色对
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
}

void Draw()
{                                            //  画图
    clear();                                 //  清屏
    for (int i = 0; i < WIDTH + 2; i++)      //  画上边框
        DrawColorMvprintw(GREEN, 0, i, "#"); //  移动光标到(0, i)位置，打印#
    for (int i = 0; i < HEIGHT + 2; i++)
    { //  画左右边框
        for (int j = 0; j < WIDTH + 2; j++)
        {
            if (j == 0)
                DrawColorMvprintw(GREEN, i + 1, j, "#");
            if (i == y && j == x)
                DrawColorMvprintw(YELLOW, i + 1, j + 1, "@");
            else if (i == fruitY && j == fruitX)
                DrawColorMvprintw(RED, i + 1, j + 1, "F");
            else
            {
                int print = 0;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        DrawColorMvprintw(YELLOW, i + 1, j + 1, "o");
                        print = 1;
                    }
                }
                if (!print)
                    mvprintw(i + 1, j + 1, " ");
            }
            if (j == WIDTH)
                DrawColorMvprintw(GREEN, i + 1, j + 1, "#");
        }
    }
    for (int i = 0; i < WIDTH + 2; i++) //  画下边框
        DrawColorMvprintw(GREEN, HEIGHT + 1, i, "#");
    DrawColorMvprintw(BLUE, HEIGHT + 2, 0, "Score: %d", score); //  打印分数
    refresh();                                                  //  刷新屏幕
}

void Input()
{ //  输入
    int c = getch();
    switch (c)
    {
    case 'a':
        dir = LEFT; //  输入a，蛇向左移动
        break;
    case 'd':
        dir = RIGHT;
        break;
    case 'w':
        dir = UP;
        break;
    case 's':
        dir = DOWN;
        break;
    case 'x':
        endwin();
        exit(0);
    }
}

void Logic()
{                         //  逻辑
    int prevX = tailX[0]; //  记录蛇尾的位置
    int prevY = tailY[0]; //  记录蛇尾的位置
    int prev2X, prev2Y;   //  记录蛇尾的位置
    tailX[0] = x;         //  更新蛇尾的位置
    tailY[0] = y;         //  更新蛇尾的位置
    for (int i = 1; i < nTail; i++)
    {                      //  更新蛇的位置
        prev2X = tailX[i]; //  记录蛇尾的位置
        prev2Y = tailY[i]; //  记录蛇尾的位置
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
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

    if (x >= WIDTH)
        x = 0;
    else if (x < 0)
        x = WIDTH - 1;
    if (y >= HEIGHT)
        y = 0;
    else if (y < 0)
        y = HEIGHT - 1;

    for (int i = 0; i < nTail; i++)
    {
        if (tailX[i] == x && tailY[i] == y)
        {
            clear();
            DrawColorMvprintw(RED, HEIGHT / 2, (WIDTH - 10) / 2, "Game Over!");
            DrawColorMvprintw(BLUE, HEIGHT / 2 + 1, (WIDTH - 10) / 2, "Score: %d", score);
            refresh();
            DrawColorMvprintw(GREEN, HEIGHT / 2 + 2, (WIDTH - 10) / 2, "Press Enter to exit");
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

    if (x == fruitX && y == fruitY)
    {
        score += 10;
        // 确保水果的位置不与蛇的位置重叠
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
        nTail++;
    }
}

int main()
{
    Setup(); // 初始化
    while (1)
    {                  // 游戏循环
        Draw();        // 画图
        Input();       // 输入
        Logic();       // 逻辑

        //根据得分动态调整游戏速度
        int speed = 80 - score / 10;
        if (speed < 30) speed = 30;
        sleep_ms(speed); // 等待
    }
    endwin(); // 结束
    return 0;
}
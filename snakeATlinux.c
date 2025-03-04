#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

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
}

void Draw()
{                                            //  画图
    clear();                                 //  清屏
    for (int i = 0; i < WIDTH + 2; i++)      //  画上边框
        mvprintw( 0, i, "#"); //  移动光标到(0, i)位置，打印#
    for (int i = 0; i < HEIGHT + 2; i++)
    { //  画左右边框
        for (int j = 0; j < WIDTH + 2; j++)
        {
            if (j == 0)
                mvprintw( i + 1, j, "#");
            if (i == y && j == x)
                mvprintw( i + 1, j + 1, "@");
            else if (i == fruitY && j == fruitX)
                mvprintw( i + 1, j + 1, "F");
            else
            {
                int print = 0;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        mvprintw( i + 1, j + 1, "o");
                        print = 1;
                    }
                }
                if (!print)
                    mvprintw(i + 1, j + 1, " ");
            }
            if (j == WIDTH)
                mvprintw( i + 1, j + 1, "#");
        }
    }
    for (int i = 0; i < WIDTH + 2; i++) //  画下边框
        mvprintw( HEIGHT + 1, i, "#");
    mvprintw( HEIGHT + 2, 0, "Score: %d", score); //  打印分数
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
            mvprintw( HEIGHT / 2, (WIDTH - 10) / 2, "Game Over!");
            mvprintw( HEIGHT / 2 + 1, (WIDTH - 10) / 2, "Score: %d", score);
            refresh();
            mvprintw( HEIGHT / 2 + 2, (WIDTH - 10) / 2, "Press Enter to exit");
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

        usleep(100000); // 等待
    }
    endwin(); // 结束
    return 0;
}
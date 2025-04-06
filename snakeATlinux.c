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

// int x, y, fruitX, fruitY, score; //  x和y是蛇的头，fruitX和fruitY是水果，score是游戏的分数
// int tailX[100], tailY[100];      //  蛇的尾巴 x和y坐标
// int nTail;                       //  蛇的长度

// 定义方向枚举，用于表示不同方向
typedef enum eDirection
{
    STOP = 0,        // 停止
    LEFT,            // 左
    RIGHT,           // 右
    UP,              // 上
    DOWN             // 下
} eDirection;        //  枚举方向
eDirection role_dir; //  方向变量，用于存储当前方向

typedef enum eColor
{
    Role_COLOR_BLACK = 1, // 黑色
    Role_COLOR_RED,       // 蓝色
    Role_COLOR_GREEN,     // 青色
    Role_COLOR_YELLOW,    // 绿色
    Role_COLOR_BLUE,      // 红色
    Role_COLOR_MAGENTA,   // 紫色
    Role_COLOR_CYAN,      // 黄色
    Role_COLOR_WHITE      // 白色
} eColor;
eColor role_color; // 颜色对编号，用于存储当前颜色

// 定义角色颜色枚举，用于表示游戏中不同元素的颜色
typedef enum eRole
{
    SNAKE_HEAD = 1, // 蛇头
    SNAKE_TAIL,     // 蛇尾
    WALL,           // 墙
    FLOWER,         // 花
    SCORE,          // 分数
    STRMESSAGE      //  字符串信息
} eRole;            //  枚举角色颜色
eRole role_name;    // 角色颜色对编号，用于存储当前角色的颜色

typedef enum eValueMode
{
    VALUE_MODE_INT,    // 整数模式
    VALUE_MODE_CHAR,   // 字符模式
    VALUE_MODE_STR,    // 字符串模式
    VALUE_MODE_FLOAT,  // 浮点数模式
} eValueMode;          //  枚举值模式
eValueMode value_mode; //  值模式，用于存储当前值的模式

// 定义角色结构体，用于表示游戏中角色的位置和显示颜色
typedef struct Role
{
    bool is_show;          // 角色是否显示
    int x;                 // 角色的x坐标(不包括边界)
    int y;                 // 角色的y坐标(不包括边界)
    int name;              // 角色的类型
    int color;             // 角色的颜色
    int dir;               // 角色的方向
    int value_mode;        // 角色值的模式
    int value;             // 角色的值
    char role_message[50]; // 角色的信息
} ROLE;

// 定义蛇头的角色，初始化位置为 (0,0)，类型为 SNAKE_HEAD
ROLE snake_head = {true, 0, 0, SNAKE_HEAD, Role_COLOR_YELLOW, STOP, VALUE_MODE_CHAR, '@', ""};
// 定义蛇身的角色数组，每个元素代表一个蛇身部分的位置和类型
// 使用 GCC 的扩展语法，统一初始化所有蛇身部分的位置为 (0,0)，类型为 SNAKE_TAIL
ROLE snake_tail[100] = {
    [0 ... 99] = {true, 0, 0, SNAKE_TAIL, Role_COLOR_YELLOW, STOP, VALUE_MODE_CHAR, 'O', ""}};

// 定义墙壁的角色，初始化位置为 (0,0)，类型为 WALL
ROLE wall = {true, 0, 0, WALL, Role_COLOR_GREEN, STOP, VALUE_MODE_CHAR, '#', ""};

// 定义花朵的角色，初始化位置为 (0,0)，类型为 FLOWER
ROLE flower = {true, 0, 0, FLOWER, Role_COLOR_RED, STOP, VALUE_MODE_CHAR, 'F', ""};

// 定义游戏分数角色，初始化位置为 (0,0)，类型为 SCORE
ROLE score = {true, 0, 0, SCORE, Role_COLOR_WHITE, STOP, VALUE_MODE_INT, 0, "Score:"};

int nTail;

WINDOW *win; // 定义窗口指针，用于 ncurses 库的窗口操作


void Over(const char *message, int exit_code);
void sleep_ms(int milliseconds);
void read_config(const char *filename);

void Init_ImageBorder(); // 初始化游戏图形框架
void Draw_role(ROLE *point);
void Image_draw();


void generateFruit();
void Setup();
void Input();
void Logic();

int main()
{

    read_config("data.bin");

    Setup(); // 初始化

    while (1)
    { // 游戏循环
        Image_draw();

        Input(); // 输入
        Logic(); // 逻辑

        // 根据得分动态调整游戏速度
        int speed = SPEED - score.value / 10;
        if (speed < 30)
            speed = 30;
        sleep_ms(speed); // 等待
    }
    endwin(); // 结束
    return 0;
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
void Setup()
{ //  游戏初始化

    // 初始化游戏内容
    snake_head.dir = STOP;           //  初始方向为停止
    snake_head.x = (WIDTH - 2) / 2;  //  蛇头的初始位置
    snake_head.y = (HEIGHT - 2) / 2; //  蛇头的初始位置
    nTail = 0;                       //  初始化蛇的长度
    generateFruit();                 //  生成水果
    score.x = 1;
    score.y = HEIGHT-2;
    score.value = 0; //  初始化分数

    Init_ImageBorder(); //  初始化游戏图形框架

}
void Init_ImageBorder() // 初始化游戏图形框架
{
    // 设置窗口大小
    initscr(); // 初始化窗口
    win = newwin(HEIGHT, WIDTH, 0, 0); // 创建新窗口
    keypad(win, TRUE); // 启用键盘输入

    // 初始化游戏输入
    curs_set(0);          //  隐藏光标
    noecho();             //  输入不显示在屏幕上
    cbreak();              //  禁用行缓冲
    nodelay(stdscr, TRUE); // 设置非阻塞输入模式

    // 初始化颜色
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();        //  开启颜色
    use_default_colors(); //  使用默认颜色
    // 根据不同的背景颜色，初始化角色颜色对
    init_pair(Role_COLOR_BLACK, COLOR_BLACK, BACK_G);
    init_pair(Role_COLOR_RED, COLOR_RED, BACK_G);
    init_pair(Role_COLOR_GREEN, COLOR_GREEN, BACK_G);
    init_pair(Role_COLOR_YELLOW, COLOR_YELLOW, BACK_G);
    init_pair(Role_COLOR_BLUE, COLOR_BLUE, BACK_G);
    init_pair(Role_COLOR_WHITE, COLOR_WHITE, BACK_G);
    init_pair(Role_COLOR_MAGENTA, COLOR_MAGENTA, BACK_G);
    init_pair(Role_COLOR_CYAN, COLOR_CYAN, BACK_G);

    clear();              //  清屏
}
void Draw_role(ROLE *point) // 按照ROLE结构体中的位置,颜色,和字符信息在ImageF中绘制
{

    if (point->is_show == true)
    {
        switch (point->value_mode)
        {
        case VALUE_MODE_INT:
                int len = snprintf(NULL, 0, "%s%d", point->role_message, point->value);
                char *str = (char *)malloc(len + 1);
                snprintf(str, len + 1, "%s%d", point->role_message, point->value);
        
                wattron(win,COLOR_PAIR(point->color));
                mvwaddstr(win,point->y + 1, point->x + 1, str);
                wattroff(win,COLOR_PAIR(point->color));
        
                free(str);
            break;
        case VALUE_MODE_CHAR:
            wattron(win,COLOR_PAIR(point->color));
            mvwaddch(win,point->y + 1, point->x + 1, point->value);
            wattroff(win,COLOR_PAIR(point->color));
            break;
        case VALUE_MODE_STR:
            wattron(win,COLOR_PAIR(point->color));
            mvwaddstr(win,point->y + 1, point->x + 1, point->role_message);
            wattroff(win,COLOR_PAIR(point->color));
            break;
        default:
            break;
        }
    }
    return;
}

void Image_draw()
{

    wresize(win, HEIGHT, WIDTH);
    mvwin(win, 0, 0);

    wclear(win);
    wattron(win,COLOR_PAIR(Role_COLOR_GREEN));
    wborder (win,'#', '#', '#', '#', '#', '#', '#', '#'); // 绘制边框
    wattroff(win,COLOR_PAIR(Role_COLOR_GREEN));

    wattron(win,COLOR_PAIR(Role_COLOR_YELLOW));
    mvwaddstr(win,0,3,"Snake Game");
    wattroff(win,COLOR_PAIR(Role_COLOR_YELLOW));

    Draw_role(&snake_head);
    for (int i = 0; i < nTail; i++)
    {
        Draw_role(&snake_tail[i]);
    }
    Draw_role(&flower);
    Draw_role(&score);
    wrefresh(win);

    //refresh();
}


void Input()
{ //  输入
    int c;
    while ((c = getch()) != ERR) // 循环读取输入，清空缓冲区
    {
        switch (c)
        {
        case 'a':
            if (snake_head.dir != RIGHT)
                snake_head.dir = LEFT; // 防止蛇反向移动
            break;
        case 'd':
            if (snake_head.dir != LEFT)
                snake_head.dir = RIGHT;
            break;
        case 'w':
            if (snake_head.dir != DOWN)
                snake_head.dir = UP;
            break;
        case 's':
            if (snake_head.dir != UP)
                snake_head.dir = DOWN;
            break;
        case 'x':
            Over("Bye Bye!", 0);
            break;
        default:
            break;
        }
    }
}
void Logic()
{                                   //  逻辑
    int prevX = snake_tail[0].x;    //  记录蛇尾的位置
    int prevY = snake_tail[0].y;    //  记录蛇尾的位置
    int prev2X, prev2Y;             //  记录蛇尾的位置
    snake_tail[0].x = snake_head.x; //  更新蛇尾的位置
    snake_tail[0].y = snake_head.y; //  更新蛇尾的位置
    //  更新蛇尾的位置
    for (int i = 1; i < nTail; i++)
    {                             //  更新蛇的位置
        prev2X = snake_tail[i].x; //  记录蛇尾的位置
        prev2Y = snake_tail[i].y; //  记录蛇尾的位置
        snake_tail[i].x = prevX;
        snake_tail[i].y = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // 根据方向更新蛇头的位置
    switch (snake_head.dir)
    {
    case LEFT:
        snake_head.x--;
        break;
    case RIGHT:
        snake_head.x++;
        break;
    case UP:
        snake_head.y--;
        break;
    case DOWN:
        snake_head.y++;
        break;
    default:
        break;
    }

    // 如果蛇头碰到边界，从另一边出来
    if (snake_head.x >= WIDTH - 2)
        snake_head.x = 0;
    else if (snake_head.x < 0)
        snake_head.x = WIDTH - 3;
    if (snake_head.y >= HEIGHT - 2)
        snake_head.y = 0;
    else if (snake_head.y < 0)
        snake_head.y = HEIGHT - 3;

    // 如果蛇头碰到蛇身，游戏结束
    for (int i = 0; i < nTail; i++)
    {
        if (snake_tail[i].x == snake_head.x && snake_tail[i].y == snake_head.y)
        {
            Over("Game Over!", 0);

            break;
        }
    }

    // 如果蛇头碰到水果，增加分数，并生成新的水果位置
    if (snake_head.x == flower.x && snake_head.y == flower.y)
    {
        score.value += 10;
        // 确保水果的位置不与蛇的位置重叠
        generateFruit();
        nTail++;
    }
}
// 定义一个函数来替换 usleep
void sleep_ms(int milliseconds)
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
// 生成花朵
void generateFruit()
{
    srand(time(NULL));
    int validPosition = 0;
    while (!validPosition)
    {
        flower.x = rand() % (WIDTH - 2);
        flower.y = rand() % (HEIGHT - 2);
        validPosition = 1;
        for (int i = 0; i < nTail; i++)
        {
            if (snake_tail[i].x == flower.x && snake_tail[i].y == flower.y)
            {
                validPosition = 0;
                break;
            }
        }
    }
}

void Over(const char *message, int exit_code)
{
    // 清空绘图板

    wclear(win);
    // 显示游戏结束信息
    wresize(win, HEIGHT, WIDTH);
    mvwin(win, 0, 0);

    wclear(win);
    wattron(win,COLOR_PAIR(Role_COLOR_GREEN));
    wborder (win,'#', '#', '#', '#', '#', '#', '#', '#'); // 绘制边框
    wattroff(win,COLOR_PAIR(Role_COLOR_GREEN));

    wattron(win,COLOR_PAIR(Role_COLOR_YELLOW));
    mvwaddstr(win,0,3,"Snake Game");
    wattroff(win,COLOR_PAIR(Role_COLOR_YELLOW));

    // 在窗口中显示退出信息
    if (message != NULL)
    {
        ROLE strmessage_message = {true, 0, 0, STRMESSAGE, Role_COLOR_YELLOW, STOP, VALUE_MODE_STR, 0, ""};
        strmessage_message.x = (WIDTH - 4 - strlen(message)) / 2;
        strmessage_message.y = (HEIGHT - 1) / 2 - 3; // 消息在屏幕中间
        strcpy(strmessage_message.role_message, message);
        Draw_role(&strmessage_message);
    }

    ROLE strmessage_score = {true, 0, 0, SCORE, Role_COLOR_RED, STOP, VALUE_MODE_INT, score.value, "Score:"};
    strmessage_score.x = (WIDTH - 2 - 10) / 2;
    strmessage_score.y = (HEIGHT - 1) / 2 - 1; // 分数在消息下方
    Draw_role(&strmessage_score);

    // 显示提示信息
    const char *prompt = "Press Enter to exit";
    ROLE strmessage = {true, 0, 0, STRMESSAGE, Role_COLOR_WHITE, STOP, VALUE_MODE_STR, 0, ""};
    strcpy(strmessage.role_message, prompt);
    strmessage.x = (WIDTH - 2 - strlen(prompt)) / 2;
    strmessage.y = (HEIGHT - 1) / 2 + 1; // 提示信息在消息下方
    Draw_role(&strmessage);
    // 刷新屏幕
    wrefresh(win);
    strmessage.is_show = false;

    // 等待用户按下回车键
    timeout(-1); // 禁用输入超时
    while (getch() != '\n')
        ;

    // 清除窗口
    if(win != NULL)
    {
        delwin(win);
        win = NULL;
    }
    // 恢复终端模式
    if (!isendwin())
    {
        endwin();
    }

    // 退出程序
    exit(exit_code);
}


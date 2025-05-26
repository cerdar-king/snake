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
    // 可以根据需要添加更多颜色
} eColor;
eColor role_color; // 颜色对编号，用于存储当前颜色

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
    //    bool is_show;          // 角色是否显示
    int type;      // 角色的类型
    char name[30]; // 角色的名称
    int Index;     // 角色的索引

    bool is_show;   // 角色是否显示
    int x;          // 角色的x坐标(不包括边界)
    int y;          // 角色的y坐标(不包括边界)
    int speed;      // 角色的速度
    eDirection dir; // 角色的方向

    eColor color;          // 角色的颜色
    eValueMode value_mode; // 角色值的模式
    int value;             // 角色的值
    char role_message[50]; // 角色的信息
} ROLE;

typedef struct RoleNode
{
    ROLE role; // 角色的外在表现形式

    // 角色的行为模式，如何根据自身的逻辑改变自身的外在表现形式
    void (*logic_func)(struct RoleNode *self); // 角色的逻辑处理函数指针
    // 角色的行为模式，如何根据自身的逻辑改变自身的外在表现形式
    // 是否移动，是否显示 ，是否受控制,被谁控制,（input1,input2,input3,computer

    // 角色的链表链接纽带
    struct RoleNode *prev;       // 上一个节点
    struct RoleNode *next;       // 下一个主节点
    struct RoleNode *anotherCPY; // 另一个同类节点
    struct RoleNode *childNode;  // 关联的子结点
} ROLE_NODE;

ROLE_NODE *head = NULL; // 角色链表头指针
ROLE_NODE *tail = NULL; // 角色链表尾指针

int *control1;
int *control2;
int *control3;
int *control4;

WINDOW *win; // 定义窗口指针，用于 ncurses 库的窗口操作

void Over(const char *message, int exit_code);
void sleep_ms(int milliseconds);
void read_config(const char *filename);

void Init_ImageBorder(); // 初始化游戏图形框架
void Draw_role(ROLE *point);
void Image_draw();
void snakeHeadLogic(ROLE_NODE *self);
void flowerLogic(ROLE_NODE *self);

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

void Setup()
{ //  游戏初始化

    // 初始化游戏内容
    snake_head.dir = STOP;           //  初始方向为停止
    snake_head.x = (WIDTH - 2) / 2;  //  蛇头的初始位置
    snake_head.y = (HEIGHT - 2) / 2; //  蛇头的初始位置
    nTail = 0;                       //  初始化蛇的长度
    generateFruit();                 //  生成水果
    score.x = 1;
    score.y = HEIGHT - 2;
    score.value = 0; //  初始化分数

    Init_ImageBorder(); //  初始化游戏图形框架
}
void Init_ImageBorder() // 初始化游戏图形框架
{
    // 设置窗口大小
    initscr();                         // 初始化窗口
    win = newwin(HEIGHT, WIDTH, 0, 0); // 创建新窗口
    keypad(win, TRUE);                 // 启用键盘输入

    // 初始化游戏输入
    curs_set(0);           //  隐藏光标
    noecho();              //  输入不显示在屏幕上
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
    init_pair(Role_COLOR_BLACK, COLOR_BLACK, -1);
    init_pair(Role_COLOR_RED, COLOR_RED, -1);
    init_pair(Role_COLOR_GREEN, COLOR_GREEN, -1);
    init_pair(Role_COLOR_YELLOW, COLOR_YELLOW, -1);
    init_pair(Role_COLOR_BLUE, COLOR_BLUE, -1);
    init_pair(Role_COLOR_WHITE, COLOR_WHITE, -1);
    init_pair(Role_COLOR_MAGENTA, COLOR_MAGENTA, -1);
    init_pair(Role_COLOR_CYAN, COLOR_CYAN, -1);

    clear(); //  清屏
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

            wattron(win, COLOR_PAIR(point->color));
            mvwaddstr(win, point->y + 1, point->x + 1, str);
            wattroff(win, COLOR_PAIR(point->color));

            free(str);
            break;
        case VALUE_MODE_CHAR:
            wattron(win, COLOR_PAIR(point->color));
            mvwaddch(win, point->y + 1, point->x + 1, point->value);
            wattroff(win, COLOR_PAIR(point->color));
            break;
        case VALUE_MODE_STR:
            wattron(win, COLOR_PAIR(point->color));
            mvwaddstr(win, point->y + 1, point->x + 1, point->role_message);
            wattroff(win, COLOR_PAIR(point->color));
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
    wattron(win, COLOR_PAIR(Role_COLOR_GREEN));
    wborder(win, '#', '#', '#', '#', '#', '#', '#', '#'); // 绘制边框
    wattroff(win, COLOR_PAIR(Role_COLOR_GREEN));

    wattron(win, COLOR_PAIR(Role_COLOR_YELLOW));
    mvwaddstr(win, 0, 3, "Snake Game");
    wattroff(win, COLOR_PAIR(Role_COLOR_YELLOW));

    Draw_role(&snake_head);
    for (int i = 0; i < nTail; i++)
    {
        Draw_role(&snake_tail[i]);
    }
    Draw_role(&flower);
    Draw_role(&score);
    wrefresh(win);

    // refresh();
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
    wattron(win, COLOR_PAIR(Role_COLOR_GREEN));
    wborder(win, '#', '#', '#', '#', '#', '#', '#', '#'); // 绘制边框
    wattroff(win, COLOR_PAIR(Role_COLOR_GREEN));

    wattron(win, COLOR_PAIR(Role_COLOR_YELLOW));
    mvwaddstr(win, 0, 3, "Snake Game");
    wattroff(win, COLOR_PAIR(Role_COLOR_YELLOW));

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
    if (win != NULL)
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

// 创建新节点并绑定逻辑函数
ROLE_NODE *NewRole(const char *name, int x, int y, int color, int dir, int value_mode, int value, const char *role_message, void (*logic_func)(ROLE_NODE *self))
{

    ROLE_NODE *newRole = (ROLE_NODE *)malloc(sizeof(ROLE_NODE));

    newRole->role.x = x;
    newRole->role.y = y;
    strcpy(newRole->role.name, name);
    newRole->role.color = color;
    newRole->role.dir = dir;
    newRole->role.value_mode = value_mode;
    newRole->role.value = value;
    strcpy(newRole->role.role_message, role_message);

    if (logic_func != NULL)
    {
        logic_func(newRole);              // 调用逻辑函数
        newRole->logic_func = logic_func; // 绑定逻辑函数
    }
    else
    {
        newRole->logic_func = NULL; // 绑定逻辑函数
    }

    newRole->next = NULL;
    newRole->anotherCPY = NULL; // 另一个同类节点
    newRole->childNode = NULL;  // 关联子类节点

    return newRole; // 返回新创建的角色节点指针
}
// 遍历角色节点链表并执行指定的操作
void OverRoleNode(int (*Contrl_Node_Func)(ROLE_NODE *))
{
    ROLE_NODE *current = head; // 从链表头开始遍历
    while (current != NULL)
    {
        ROLE_NODE *next = current->next; // 保存下一个节点的指针

        // 处理 anotherCPY 链接的节点
        ROLE_NODE *anotherCP = current->anotherCPY;
        while (anotherCP != NULL)
        {
            ROLE_NODE *nextAnotherCP = anotherCP->next; // 保存下一个节点的指针

            ROLE_NODE *childNod = anotherCP->childNode;
            while (childNod != NULL)
            {
                ROLE_NODE *nextChildNod = childNod->next; // 保存下一个节点的指针
                Contrl_Node_Func(childNod);               // 执行指定的操作
                childNod = nextChildNod;                  // 移动到下一个子节点
            }
            Contrl_Node_Func(anotherCP); // 执行指定的操作

            anotherCP = nextAnotherCP;
        }

        // 处理 childNode 链接的节点
        ROLE_NODE *childNod = current->childNode;
        while (childNod != NULL)
        {
            ROLE_NODE *nextChildNod = childNod->next; // 保存下一个节点的指针
            Contrl_Node_Func(childNod);               // 执行指定的操作
            childNod = nextChildNod;                  // 移动到下一个子节点
        }

        Contrl_Node_Func(current); // 执行指定的操作

        // 移动到下一个主链表节点
        current = next;
    }
}

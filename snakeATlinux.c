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

// 定义角色类型，用于表示游戏中不同角色元素
typedef enum eRole
{
    SNAKE_HEAD = 1, // 蛇头
    SNAKE_TAIL,     // 蛇尾
    WALL,           // 墙
    FLOWER,         // 花
    STRMESSAGE      //  字符串信息
} eRole;
eRole role_type;

//**************************************************
int global_id_counter = 1000; // 全局 ID 计数器
int score = 0;                // 分数变量

// 定义方向枚举，用于表示不同方向
typedef enum eDirection
{
    STOP = 0, // 停止
    LEFT,     // 左
    RIGHT,    // 右
    UP,       // 上
    DOWN      // 下

    // 其他方向可以根据需要添加
    // 例如：UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
    // 也可以使用更复杂的方向表示法，例如使用角度或弧度
    // 但在这个简单的贪吃蛇游戏中，四个基本方向就足够了
} eDirection; //  枚举方向

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

    int Role_type;      // 角色的类型编号
    char Role_name[30]; // 角色的类型
    int Role_ID;        // 角色编号

    bool is_show; // 角色是否显示
    int x;        // 角色的x坐标(不包括边界)
    int y;        // 角色的y坐标(不包括边界)

    int old_x;//  角色移动前的旧x坐标
    int old_y;//  角色移动前的旧y坐标

    int speed;      // 角色的速度
    eDirection dir; // 角色的方向

    eColor color;          // 角色的颜色
    eValueMode value_mode; // 角色值的模式

    int value;             // 角色的值
    char role_message[50]; // 角色的信息

    // 角色的行为模式，如何根据自身的逻辑改变自身的外在表现形式
    // 是否移动，是否显示 ，是否受控制,被谁控制,（input1,input2,input3,computer

} ROLE;

typedef struct RoleNode
{
    ROLE role;                                 // 角色的外在表现形式
    void (*logic_func)(struct RoleNode *self); // 角色的逻辑处理函数指针

    // 角色的链表链接纽带
    struct RoleNode *next;       // 下一个兄弟节点
    struct RoleNode *prev;       // 前一个兄弟节点
    struct RoleNode *child_head; // 关联的子结点头
    struct RoleNode *parent;     // 关联的父节点
} ROLE_NODE;

ROLE_NODE *head = NULL; // 角色链表头指针
ROLE_NODE *tail = NULL; // 角色链表尾指针

ROLE_NODE *control1 = NULL;//预设的控制指针用来给可控角色发送控制指令
ROLE_NODE *control2 = NULL;
ROLE_NODE *control3 = NULL;
ROLE_NODE *control4 = NULL;

//*******************************************************************************

WINDOW *win; // 定义窗口指针，用于 ncurses 库的窗口操作

void Over(const char *message, int exit_code);//退出信息界面的绘制函数
void sleep_ms(int milliseconds);//延时函数
void read_config(const char *filename);//读取配置文件函数

void Init_ImageBorder(); // 初始化游戏图形框架
void Draw_role(ROLE *point);// 绘制角色函数
void Image_draw();// 绘制所有游戏元素函数

void snakeHeadLogic(ROLE_NODE *self);// 蛇头自身逻辑处理函数
void snakeBodyLogic(ROLE_NODE *self);// 蛇身自身逻辑处理函数
void scoreLogic(ROLE_NODE *self);// 分数逻辑处理函数

int checkCollision(ROLE_NODE *self);// 检查碰撞函数
ROLE_NODE *InitRole(int type, const char *name,
                    bool is_show, int x, int y, int speed, int dir, int color,
                    int value_mode, int value, const char *role_message,
                    void (*logic_func)(ROLE_NODE *self));// 创建新节点并绑定逻辑函数
ROLE_NODE *InitChildRole(ROLE_NODE *parent,
                         int type, const char *name,
                         bool is_show, int x, int y, int speed, int dir, int color,
                         int value_mode, int value, const char *role_message,
                         void (*logic_func)(ROLE_NODE *self));// 创建新子节点并绑定逻辑函数
void generateFruit();// 生成花朵函数
void Setup();// 游戏初始化函数
void Input();// 接受键盘输入函数
void OverLogic();//遍历链表节点逻辑函数

// 遍历链表节点函数
int traverseNodes(ROLE_NODE *head, ROLE_NODE *check,
                  int (*callback)(ROLE_NODE *node,
                                  ROLE_NODE *check_node));

// 回调函数
int drawRoleCallback(ROLE_NODE *node,
                     ROLE_NODE *check_node);// 绘制角色回调函数
int logicCallback(ROLE_NODE *node,
                  ROLE_NODE *check_node);// 角色逻辑处理回调函数
int CollisionCallback(ROLE_NODE *node,
                      ROLE_NODE *check_node);// 碰撞检测回调函数
int destroyRoleByIDCallback(ROLE_NODE *node,
                            ROLE_NODE *check_node);// 删除角色回调函数
int main()
{

    read_config("data.bin");

    Setup(); // 初始化

    while (1)
    { // 游戏循环
        Image_draw();

        Input(); // 输入

        OverLogic();

        // 根据得分动态调整游戏速度
        sleep_ms(200); // 等待
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
    control1 = InitRole(SNAKE_HEAD, "snake_head", true, (WIDTH - 1) / 2, (HEIGHT - 2) / 2, 0, STOP, Role_COLOR_YELLOW, VALUE_MODE_CHAR, '@', "Snake", snakeHeadLogic);

    InitRole(STRMESSAGE, "score", true, 1, HEIGHT - 2, 0, STOP, Role_COLOR_YELLOW, VALUE_MODE_INT, score, "Score:", scoreLogic);
    // 初始化蛇头
    generateFruit();    // 生成花朵
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

    // 绘制游戏元素
    // 遍历主链表
    traverseNodes(head, NULL, drawRoleCallback);

    wrefresh(win);
}
void Input()
{ //  输入
    int c;
    while ((c = getch()) != ERR) // 循环读取输入，清空缓冲区
    {
        switch (c)
        {
        case 'a':
            if (control1->role.dir != RIGHT)
                control1->role.dir = LEFT; // 防止蛇反向移动
            break;
        case 'd':
            if (control1->role.dir != LEFT)
                control1->role.dir = RIGHT;
            break;
        case 'w':
            if (control1->role.dir != DOWN)
                control1->role.dir = UP;
            break;
        case 's':
            if (control1->role.dir != UP)
                control1->role.dir = DOWN;
            break;
        case 'x':
            Over("Bye Bye!", 0);
            break;
        default:
            break;
        }
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
    int a, x, y;
    do
    {
        // 生成随机的 x 坐标
        x = rand() % (WIDTH - 2);  // 生成随机的 x 坐标
        y = rand() % (HEIGHT - 2); // 生成随机的 y 坐标
        ROLE_NODE current;
        current.role.x = x;
        current.role.y = y;
        a = checkCollision(&current);
    } while (a); // 确保花朵的位置不与蛇的位置重叠需要通过碰撞测试确定花朵没有和蛇重叠没有种到屏幕边框上

    // 如果x，y位置没问题则生成花朵
    InitRole(FLOWER, "flower", true, x, y, 0, STOP, Role_COLOR_RED, VALUE_MODE_CHAR, 'F', "Flower", NULL);
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
        ROLE strmessage_message = {
            .Role_type = STRMESSAGE,      // 角色类型
            .Role_name = "",              // 角色名称（如果不需要，可以留空）
            .Role_ID = 0,                 // 角色编号
            .is_show = TRUE,              // 是否显示
            .x = 0,                       // x 坐标
            .y = 0,                       // y 坐标
            .speed = 0,                   // 速度
            .dir = STOP,                  // 方向
            .color = Role_COLOR_BLUE,     // 颜色
            .value_mode = VALUE_MODE_STR, // 值模式
            .value = 0,                   // 值
            .role_message = ""            // 角色信息
        };

        // 设置角色信息
        strmessage_message.x = (WIDTH - 4 - strlen(message)) / 2;
        strmessage_message.y = (HEIGHT - 1) / 2 - 3; // 消息在屏幕中间
        strcpy(strmessage_message.role_message, message);
        Draw_role(&strmessage_message);
    }

    ROLE strmessage_score = {
        .Role_type = STRMESSAGE,      // 角色类型
        .Role_name = "",              // 角色名称（如果不需要，可以留空）
        .Role_ID = 0,                 // 角色编号
        .is_show = TRUE,              // 是否显示
        .x = 0,                       // x 坐标
        .y = 0,                       // y 坐标
        .speed = 0,                   // 速度
        .dir = STOP,                  // 方向
        .color = Role_COLOR_RED,      // 颜色
        .value_mode = VALUE_MODE_INT, // 值模式
        .value = score,               // 值
        .role_message = "score:"      // 角色信息
    };

    strmessage_score.x = (WIDTH - 2 - 10) / 2;
    strmessage_score.y = (HEIGHT - 1) / 2 - 1; // 分数在消息下方
    Draw_role(&strmessage_score);

    // 显示提示信息
    const char *prompt = "Press Enter to exit";
    ROLE strmessage = {
        .Role_type = STRMESSAGE,      // 角色类型
        .Role_name = "",              // 角色名称（如果不需要，可以留空）
        .Role_ID = 0,                 // 角色编号
        .is_show = TRUE,              // 是否显示
        .x = 0,                       // x 坐标
        .y = 0,                       // y 坐标
        .speed = 0,                   // 速度
        .dir = STOP,                  // 方向
        .color = Role_COLOR_YELLOW,   // 颜色
        .value_mode = VALUE_MODE_STR, // 值模式
        .value = 0,                   // 值
        .role_message = ""            // 角色信息
    };

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
ROLE_NODE *InitRole(int type, const char *name,
                    bool is_show, int x, int y, int speed, int dir, int color,
                    int value_mode, int value, const char *role_message,
                    void (*logic_func)(ROLE_NODE *self))
{
    // 分配内存
    ROLE_NODE *newRole = (ROLE_NODE *)malloc(sizeof(ROLE_NODE));
    if (newRole == NULL)
    {
        perror("Failed to allocate memory for new role node");
        exit(EXIT_FAILURE);
    }

    // 初始化链表节点
    newRole->prev = NULL;
    newRole->next = NULL;
    newRole->child_head = NULL;
    newRole->parent = NULL;

    // 将新节点添加到链表中
    if (head == NULL)
    {
        head = newRole; // 如果链表为空，则将新节点设置为头节点
        tail = newRole; // 将新节点设置为尾节点
    }
    else
    {
        tail->next = newRole; // 将新节点添加到链表末尾
        newRole->prev = tail; // 设置新节点的前驱节点
        tail = newRole;       // 更新尾节点
    }

    // 设置新节点的属性
    newRole->role.Role_type = type;

    // 确保字符串不会超出目标数组大小
    strncpy(newRole->role.Role_name, name, sizeof(newRole->role.Role_name) - 1);
    newRole->role.Role_name[sizeof(newRole->role.Role_name) - 1] = '\0'; // 确保以 '\0' 结尾

    newRole->role.Role_ID = global_id_counter++;

    newRole->role.is_show = is_show;
    newRole->role.x = x;
    newRole->role.y = y;

    newRole->role.old_x = x;
    newRole->role.old_y = y;

    newRole->role.speed = speed;
    newRole->role.dir = dir;
    newRole->role.color = color;
    newRole->role.value_mode = value_mode;
    newRole->role.value = value;

    // 确保字符串不会超出目标数组大小
    strncpy(newRole->role.role_message, role_message, sizeof(newRole->role.role_message) - 1);
    newRole->role.role_message[sizeof(newRole->role.role_message) - 1] = '\0'; // 确保以 '\0' 结尾

    // 绑定逻辑函数
    newRole->logic_func = logic_func;

    return newRole; // 返回新节点的指针
}

ROLE_NODE *InitChildRole(ROLE_NODE *parent,
                         int type, const char *name,
                         bool is_show, int x, int y, int speed, int dir, int color,
                         int value_mode, int value, const char *role_message,
                         void (*logic_func)(ROLE_NODE *self))
{
    // 分配内存
    ROLE_NODE *child = (ROLE_NODE *)malloc(sizeof(ROLE_NODE));
    if (child == NULL)
    {
        perror("Failed to allocate memory for new child role node");
        exit(EXIT_FAILURE);
    }

    // 初始化链表节点
    child->prev = NULL;
    child->next = NULL;
    child->child_head = NULL;
    child->parent = NULL;

    if (parent == NULL)
    {
        perror("Parent node is NULL");
        free(child);
        return NULL;
    }

    

    if (parent->child_head == NULL)
    {
        parent->child_head = child; // 如果父节点没有子节点，则直接添加
        child->parent = parent; // 设置子节点的父节点
    }
    else
    {
        ROLE_NODE *current = parent->child_head;
        while (current->next != NULL)
        {
            current = current->next; // 找到子节点链表的末尾
        }
        current->next = child; // 添加到子节点链表末尾
        child->prev = current;
    }

    // 设置新节点的属性
    child->role.Role_type = type;

    // 确保字符串不会超出目标数组大小
    strncpy(child->role.Role_name, name, sizeof(child->role.Role_name) - 1);
    child->role.Role_name[sizeof(child->role.Role_name) - 1] = '\0'; // 确保以 '\0' 结尾

    child->role.Role_ID = global_id_counter++;

    child->role.is_show = is_show;
    child->role.x = x;
    child->role.y = y;

    child->role.old_x = x;
    child->role.old_y = y;

    child->role.speed = speed;
    child->role.dir = dir;
    child->role.color = color;
    child->role.value_mode = value_mode;
    child->role.value = value;

    // 确保字符串不会超出目标数组大小
    strncpy(child->role.role_message, role_message, sizeof(child->role.role_message) - 1);
    child->role.role_message[sizeof(child->role.role_message) - 1] = '\0'; // 确保以 '\0' 结尾

    // 绑定逻辑函数
    child->logic_func = logic_func;

    return child; // 返回新节点的指针
}

int checkCollision(ROLE_NODE *self)
{
    //  检查角色是否超出屏幕边界
    if (self->role.x >= WIDTH - 2)
    {
        self->role.x = 0;
    }
    else if (self->role.x < 0)
    {
        self->role.x = WIDTH - 3;
    }
    if (self->role.y >= HEIGHT - 2)
    {
        self->role.y = 0;
    }
    else if (self->role.y < 0)
    {
        self->role.y = HEIGHT - 3;
    }
    // 遍历主链表
    return traverseNodes(head, self, CollisionCallback);
    // return 0;
}

void snakeHeadLogic(ROLE_NODE *self)
{
    // 更新蛇头的位置
    self->role.old_x = self->role.x;
    self->role.old_y = self->role.y;

    switch (self->role.dir)
    {
    case LEFT:
        self->role.x--;
        break;
    case RIGHT:
        self->role.x++;
        break;
    case UP:
        self->role.y--;
        break;
    case DOWN:
        self->role.y++;
        break;
    default:
        break;
    }

    // 检查碰撞
    int collision = 0;
    collision = checkCollision(self);
    if (collision != 0)
    {
        // 处理碰撞
        if (collision / 10000 == SNAKE_TAIL)
        {
            // 蛇头碰到蛇身，游戏结束
            Over("Game Over!", 0);
        }
        else if (collision / 10000 == FLOWER)
        {
            // 蛇头碰到花朵，增加分数，并生成新的花朵位置
            score += 10; // 加分

            // 删除花朵
            ROLE_NODE flowerNode;
            flowerNode.role.Role_ID = collision % 10000;               // 获取花朵节点
            traverseNodes(head, &flowerNode, destroyRoleByIDCallback); // 删除花朵

            // 生成新的蛇身

            InitChildRole(self, SNAKE_TAIL, "snake_tail", true, self->role.old_x, self->role.old_y, 0, STOP, Role_COLOR_YELLOW, VALUE_MODE_CHAR, 'o', "Snake", snakeBodyLogic); // 增加蛇身

            // 生成新的花朵
            generateFruit(); // 生成新的花朵
        }
    }
}

void snakeBodyLogic(ROLE_NODE *self)
{
    self->role.old_x = self->role.x;
    self->role.old_y = self->role.y;

    // 更新蛇身的位置
    if (self->parent != NULL)
    {
        self->role.x = self->parent->role.old_x;
        self->role.y = self->parent->role.old_y;
    }
    else
    {
        self->role.x = self->prev->role.old_x;
        self->role.y = self->prev->role.old_y;
    }
}
void scoreLogic(ROLE_NODE *self)
{
    // 更新分数
    self->role.value = score;
}
void OverLogic()
{
    // 遍历主链表
    traverseNodes(head, NULL, logicCallback);
}

// 当需要遍历整个链表时，使用这个函数，需要传入一个特定功能的回调函数
// 该函数会遍历整个链表，并对每个节点执行回调函数
// 该函数的参数包括链表头指针、回调函数指针和一个检查节点指针
// 该函数会遍历整个链表，并对每个节点执行回调函数
// 该函数会返回一个整数值，表示遍历的结果
// 遍历整个主链表和子节点链表
int traverseNodes(ROLE_NODE *head, ROLE_NODE *check,
                  int (*callback)(ROLE_NODE *node,
                                  ROLE_NODE *check_node))
{
    ROLE_NODE *current = head; // 从链表头开始遍历

    while (current != NULL)
    {
        ROLE_NODE *next = current->next; // 保存下一个节点的指针

        // 执行主节点的回调函数
        if (callback != NULL)
        {
            int result = callback(current, check);
            if (result != 0)
            {
                return result; // 如果回调函数返回非零值，则停止遍历并返回结果
            }
        }

        // 遍历子节点链表
        if (current->child_head != NULL) 
        {
            int result = traverseNodes(current->child_head, check, callback);
            if (result != 0)
            {
                return result; // 如果子节点回调函数返回非零值，则停止遍历并返回结果
            }
        }

        current = next; // 移动到下一个主节点
    }
    return 0; // 遍历完成，返回 0
}

// 回调函数
//--------------------------------------------------------------------------------------------------------
// 绘制角色
int drawRoleCallback(ROLE_NODE *node,
                     ROLE_NODE *check_node)
{
    if (node->role.is_show == true)
    {
        Draw_role(&node->role); // 绘制角色
    }
    return 0; // 返回 0，继续遍历
}

// 角色自主处理逻辑处理函数
int logicCallback(ROLE_NODE *node,
                  ROLE_NODE *check_node)
{
    if (node->logic_func != NULL)
    {
        node->logic_func(node);
        return 0; // 返回 0，继续遍历
    }
    else
    {
        return 1; // 返回非零值，停止遍历
    }
}

//  检查碰撞
int CollisionCallback(ROLE_NODE *node,
                      ROLE_NODE *check_node)
{
    // 检查碰撞
    if (node != check_node && node->role.x == check_node->role.x && node->role.y == check_node->role.y)
    {
        // 处理碰撞
        return node->role.Role_type * 10000 + node->role.Role_ID; // 返回非零值，停止遍历
    }
    return 0; // 返回 0，继续遍历
}

//  根据角色ID销毁角色节点
int destroyRoleByIDCallback(ROLE_NODE *node,
                            ROLE_NODE *check_node)
{
    // 检查角色 ID 是否匹配
    if (node->role.Role_ID == check_node->role.Role_ID)
    {
        // 释放角色节点的内存
        if (node->prev != NULL)
        {
            node->prev->next = node->next; // 更新前一个节点的 next 指针
        }
        if (node->next != NULL)
        {
            node->next->prev = node->prev; // 更新下一个节点的 prev 指针
        }
        if (node == head)
        {
            head = node->next; // 更新头指针
        }
        if (node == tail)
        {
            tail = node->prev; // 更新尾指针
        }

        // 释放角色节点的内存
        free(node);
        node = NULL;
    }
    return 0;
}


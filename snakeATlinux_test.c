#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 假设 InitRole 函数和相关结构体已经在 snakeATlinux.c 中定义
extern ROLE_NODE *InitRole(int type, const char *name,
                           bool is_show, int x, int y, int speed, int dir, int color,
                           int value_mode, int value, const char *role_message,
                           void (*logic_func)(ROLE_NODE *self));

// 全局变量声明（与主程序一致）
ROLE_NODE *head = NULL;
ROLE_NODE *tail = NULL;
static int global_id_counter = 1000;

// 测试逻辑函数
void testLogicFunc(ROLE_NODE *self) {
    printf("Logic function called for node: %s\n", self->role.Role_name);
}

// 单元测试函数
void test_InitRole() {
    printf("Running InitRole tests...\n");

    // 初始化链表为空
    head = NULL;
    tail = NULL;

    // 测试用例 1：创建第一个节点
    ROLE_NODE *node1 = InitRole(1001, "Snake Head", true, 10, 10, 1, 0, 1, 0, 0, "Head Node", testLogicFunc);
    assert(node1 != NULL); // 验证节点是否成功创建
    assert(head == node1); // 验证链表头是否正确
    assert(tail == node1); // 验证链表尾是否正确
    assert(node1->role.Role_ID == 1000); // 验证 ID 是否正确
    assert(strcmp(node1->role.Role_name, "Snake Head") == 0); // 验证名称是否正确
    assert(node1->role.x == 10 && node1->role.y == 10); // 验证坐标是否正确
    assert(node1->logic_func == testLogicFunc); // 验证逻辑函数是否正确绑定

    // 测试用例 2：创建第二个节点
    ROLE_NODE *node2 = InitRole(1002, "Flower", true, 5, 5, 0, 0, 2, 0, 0, "Flower Node", NULL);
    assert(node2 != NULL); // 验证节点是否成功创建
    assert(head == node1); // 验证链表头是否未改变
    assert(tail == node2); // 验证链表尾是否更新
    assert(node1->next == node2); // 验证链表链接是否正确
    assert(node2->role.Role_ID == 1001); // 验证 ID 是否正确
    assert(strcmp(node2->role.Role_name, "Flower") == 0); // 验证名称是否正确
    assert(node2->role.x == 5 && node2->role.y == 5); // 验证坐标是否正确
    assert(node2->logic_func == NULL); // 验证逻辑函数是否为空

    printf("All InitRole tests passed!\n");
}

int main() {
    // 运行单元测试
    test_InitRole();
    return 0;
}
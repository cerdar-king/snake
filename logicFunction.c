// Compiler: gcc (Ubuntu 11.3.0-1ubuntu1) 11.3.0

void snakeHeadLogic(ROLE_NODE *self)
{
    // 更新蛇头的位置
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
}


void snakeBodyLogic(ROLE_NODE *self)
{
    // 更新蛇身的位置
    if(self->paretsNode != NULL)
    {
        self->role.x = self->paretsNode->role.x;
        self->role.y = self->paretsNode->role.y;
    }else
    {
        self->role.x = self->prev->role.x;
        self->role.y = self->prev->role.y;
    }
}

int checkCollision(ROLE_NODE *self)
{
    //  检查角色是否超出屏幕边界
    if(self->role.x >=  WIDTH - 2)
    {
        self->role.x = 0;
    }
    else if(self->role.x < 0)
    {
        self->role.x = WIDTH - 3;
    }
    if(self->role.y >= HEIGHT - 2)
    {
        self->role.y = 0;
    }
    else if(self->role.y < 0)
    {
        self->role.y = HEIGHT - 3;
    }
    ROLE_NODE *current = head; // 从链表头开始遍历
    while (current != NULL)
    {
        ROLE_NODE *next = current->next; // 保存下一个节点的指针
        if (current != self && current->role.x == self->role.x && current->role.y == self->role.y)
        {
            // 如果当前节点与当前节点的坐标相同，则表示发生碰撞
            return self->role.Role_type * 10000 + current->role.Role_type // 返回碰撞的角色ID
        }
        // 处理 childNode 链接的节点
        ROLE_NODE *childNod = current->childNode;
        while (childNod != NULL)
        {
            ROLE_NODE *nextChildNod = childNod->next; // 保存下一个节点的指针
            if (childNod != self && childNod->role.x == self->role.x && childNod->role.y == self->role.y)
            {
                // 如果当前节点与当前节点的坐标相同，则表示发生碰撞
                return self->role.Role_type * 10000 + childNod->role.Role_type // 返回碰撞的角色ID
            }
            childNod = nextChildNod;                  // 移动到下一个子节点
        }
        
        // 移动到下一个主链表节点
        current = next;
    }
}

// 遍历角色节点链表并执行指定的操作
void OverRoleNode()
{
    ROLE_NODE *current = head; // 从链表头开始遍历
    while (current != NULL)
    {
        ROLE_NODE *next = current->next; // 保存下一个节点的指针

        // 处理 childNode 链接的节点
        ROLE_NODE *childNod = current->childNode;
        while (childNod != NULL)
        {
            ROLE_NODE *nextChildNod = childNod->next; // 保存下一个节点的指针
            childNod = nextChildNod;                  // 移动到下一个子节点
        }
        
        // 移动到下一个主链表节点
        current = next;
    }
}
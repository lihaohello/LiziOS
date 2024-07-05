#ifndef __LIST_H
#define __LIST_H

#include "types.h"

#define offset(struct_type, member) (int)(&((struct_type*)0)->member)
#define elem2entry(struct_type, struct_member_name, elem_ptr) \
    (struct_type*)((int)elem_ptr - offset(struct_type, struct_member_name))

/// @brief 双向链表的节点
struct list_elem {
    /// @brief 前驱
    struct list_elem* prev;
    /// @brief 后继
    struct list_elem* next;
};

/// @brief 双向链表
struct list {
    struct list_elem head;
    struct list_elem tail;
};

/// @brief 定义function函数
typedef bool (*function)(struct list_elem*, int arg);

void list_init(struct list*);
void list_insert_before(struct list_elem* before, struct list_elem* elem);
void list_push(struct list* plist, struct list_elem* elem);
void list_iterate(struct list* plist);
void list_append(struct list* plist, struct list_elem* elem);
void list_remove(struct list_elem* pelem);
struct list_elem* list_pop(struct list* plist);
bool list_empty(struct list* plist);
u32 list_len(struct list* plist);
struct list_elem* list_traversal(struct list* plist, function func, int arg);
bool elem_find(struct list* plist, struct list_elem* obj_elem);
#endif
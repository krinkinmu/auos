#ifndef __LIST_H__
#define __LIST_H__

#include "defs.h"

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

void list_init(struct list_head *head);
void list_add_tail(struct list_head *head, struct list_head *new_node);
void list_add_front(struct list_head *head, struct list_head *new_node);
void list_del(struct list_head *node);
int list_empty(const struct list_head *head);

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#endif /*__LIST_H__*/

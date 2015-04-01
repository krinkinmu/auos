#include <kernel/list.h>

void list_init_head(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}

static inline void list_insert_between(struct list_head *prev,
			struct list_head *next, struct list_head *node)
{
	prev->next = node;
	next->prev = node;
	node->next = next;
	node->prev = prev;
}

void list_insert_after(struct list_head *pos, struct list_head *next)
{
	list_insert_between(pos, pos->next, next);
}

void list_insert_before(struct list_head *pos, struct list_head *prev)
{
	list_insert_between(pos->prev, pos, prev);
}

static inline void list_remove_between(struct list_head *prev,
			struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

void list_remove(struct list_head *node)
{
	list_remove_between(node->prev, node->next);
}

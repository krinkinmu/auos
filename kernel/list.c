#include <list.h>

void init_list_head(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}

static inline void insert_between(struct list_head *prev,
			struct list_head *next, struct list_head *node)
{
	prev->next = node;
	next->prev = node;
	node->next = next;
	node->prev = prev;
}

void insert_after(struct list_head *pos, struct list_head *next)
{
	insert_between(pos, pos->next, next);
}

void insert_before(struct list_head *pos, struct list_head *prev)
{
	insert_between(pos->prev, pos, prev);
}

static inline void remove_between(struct list_head *prev,
			struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

void remove_from_list(struct list_head *node)
{
	remove_between(node->prev, node->next);
}

int list_empty(struct list_head *node)
{
	return node->next == node;
}

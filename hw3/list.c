#include "list.h"

void list_init(struct list_head *head)
{ head->next = head->prev = head; }

static void list_insert_between(struct list_head *prev, struct list_head *next,
			struct list_head *new_node)
{
	prev->next = new_node;
	new_node->prev = prev;
	next->prev = new_node;
	new_node->next = next;
}

void list_add_tail(struct list_head *head, struct list_head *new_node)
{ list_insert_between(head->prev, head, new_node); }

void list_add_front(struct list_head *head, struct list_head *new_node)
{ list_insert_between(head, head->next, new_node); }

void list_del(struct list_head *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

int list_empty(const struct list_head *head)
{ return head->next == head; }

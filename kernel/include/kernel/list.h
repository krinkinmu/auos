#ifndef __KERNEL_LIST_H__
#define __KERNEL_LIST_H__

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

void list_init_head(struct list_head *head);
void list_insert_after(struct list_head *pos, struct list_head *next);
void list_insert_before(struct list_head *pos, struct list_head *prev);
void list_remove(struct list_head *node);

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

#endif /*__KERNEL_LIST_H__*/

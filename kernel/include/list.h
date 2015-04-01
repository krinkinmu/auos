#ifndef __LIST_H__
#define __LIST_H__

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define INIT_LIST(name) struct list_head name = { &name, &name }

void init_list_head(struct list_head *head);
void insert_after(struct list_head *pos, struct list_head *next);
void insert_before(struct list_head *pos, struct list_head *prev);
void remove_from_list(struct list_head *node);
int list_empty(struct list_head *head);

#endif /*__LIST_H__*/

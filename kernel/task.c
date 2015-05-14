#include <kernel/page_alloc.h>
#include <kernel/utility.h>
#include <kernel/task.h>
#include <kernel/debug.h>

struct task *alloc_task(void)
{
	struct page *pages = alloc_pages(KERNEL_STACK_ORDER, ALLOC_NORMAL);

	assert(pages, "Cannot allocate task struct and stack\n");

	struct task *task = page_virtual_address(pages);

	memset(task, 0, sizeof(*task));
	return task;
}

void free_task(struct task *task)
{
	struct page *pages = pfn_to_page(phys_addr(task) >> PAGE_SHIFT);

	free_pages(pages, KERNEL_STACK_ORDER);
}

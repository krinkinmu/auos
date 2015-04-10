#include <kernel/console.h>

struct list_head consoles = { &consoles, &consoles };

void register_console(struct console *console)
{
	list_insert_before(&consoles, &console->link);
}

void unregister_console(struct console *console)
{
	list_remove(&console->link);
}

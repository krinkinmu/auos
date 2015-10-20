#ifndef __X86_GDT_H__
#define __X86_GDT_H__

#define GDT_CODE_ENTRY 1
#define GDT_DATA_ENTRY 2

#define GDT_CODE_SELECTOR ((GDT_CODE_ENTRY) << 3)
#define GDT_DATA_SELECTOR ((GDT_DATA_ENTRY) << 3)

void gdt_setup(void);

#endif /*__X86_GDT_H__*/

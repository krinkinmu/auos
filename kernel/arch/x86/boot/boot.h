#ifndef __BOOT_H__
#define __BOOT_H__

/* multiboot definitions */
#define MB_HEADER_MAGIC 0x1BADB002
#define MB_BOOT_MAGIC   0x2BADB002
#define MB_PAGE_ALIGN   0x00000001
#define MB_MEMORY_INFO  0x00000002

/* common boot definitions */
#define BOOT_TIME_STACK_SIZE 0x4000


#endif /*__BOOT_H__*/

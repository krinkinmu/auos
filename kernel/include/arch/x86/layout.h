#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#define PAGE_OFFSET 0xC0000000
#define PAGE_SIZE   0x00001000

#define pa(x) ((x) - PAGE_OFFSET)

#endif /*__LAYOUT_H__*/

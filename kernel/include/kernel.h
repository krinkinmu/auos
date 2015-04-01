#ifndef __KERNEL_H__
#define __KERNEL_H__

#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN_UP(x, a)      ALIGN_MASK(x, ((a) - 1))
#define ALIGN_DOWN(x, a)    ((x) & ~((a) - 1))

#endif /*__KERNEL_H__*/

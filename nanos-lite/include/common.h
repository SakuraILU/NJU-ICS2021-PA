#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_CTE
#define HAS_VME
//#define MULTIPROGRAM
//#define TIME_SHARING

#define USR_SPACE_ENABLE

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <debug.h>

#define PTE_R 0x002 /* Read */
#define PTE_W 0x004 /* Write */
#define PTE_X 0x008 /* Execute */

#endif

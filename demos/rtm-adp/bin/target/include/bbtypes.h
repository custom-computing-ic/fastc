
#ifndef __BBTYPES_H__
#define __BBTYPES_H__


#ifndef NULL
  #define NULL 0u
#endif
#include <bbhstdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <bbtimers.h>
typedef long unsigned u32;
typedef unsigned char u8;
typedef unsigned short u16;

typedef uint32_t bb_add32_t;
typedef uint32_t bb_size_t;
typedef uint32_t reg32_t;
typedef uint64_t reg64_t;
typedef uint32_t arg32_t;
#ifndef PE
#define PE "RT"
#endif
/**
   \brief BB address of form <sock name>@<PEtype>:<instance num>
 */
typedef struct { 
  char name[8];
  int petype;
  int inst;
} bb_sockaddr_t;


#define PRINTK(fmt,args...) bbhprintf("KERN [" PE "]: " fmt , ## args)
#define ERRORK(fmt,args...) {bbhprintf("KERN [" PE "]: error \"" __FILE__ "\" line %d " fmt , __LINE__, ## args);exit(1);}
#define WARNINGK(fmt,args...) {bbhprintf("KERN [" PE "]: warning :" fmt, ## args);}



#if defined BBKERNDEBUG
#define DBG(fmt,args...) bbhprintf("(DBG [" PE "] : " __FILE__ " line %d )" fmt,__LINE__, ## args);
#define DBG_KERN(fmt,args...) bbhprintf("([%.10Lu] DBG_KERN [" PE "]: %s line %d)" fmt,bb_get_tick(),__FILE__,__LINE__, ## args); 
#else
#define DBG(fmt,args...)
#define DBG_KERN(fmt,args...)
#endif


#define WRITE_REG(base,offset,data) *(volatile reg32_t*)((reg32_t)base + (reg32_t)offset)=(reg32_t) data  /*!< Writes a gnam_ui32 into memory. */

#define READ_REG(base,offset,data) data=((reg32_t) *(volatile reg32_t*)((reg32_t)base + (reg32_t)offset)) /*!< Writes a gnam_ui64 into memory. */

#define WRITE_REG64(base,offset,data) *(volatile reg32_t*)((reg32_t)base + (reg32_t)offset)=(reg32_t) data  /*!< Writes a gnam_ui32 into memory. */

#define READ_REG64(base,offset,data) data=((reg32_t) *(volatile reg32_t*)((reg32_t)base + (reg32_t)offset))

#ifndef MIN
#define MIN(x,y) (((x)<=(y))?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) (((x)>(y))?(x):(y))
#endif

#endif /* __BBTYPES_H__ */


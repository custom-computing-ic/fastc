#ifndef __HMAP_H__
#define __HMAP_H__
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define HMAP_DEV(var,add,size,name) 	\
  int d##var ;\
  d ##var=open(name, O_RDWR | O_SYNC);\
  if(d ##var<0){PRINTK("## cannot open " name "\n");exit(1);}\
  var =  (u32)mmap((void*) add ,size, PROT_WRITE, MAP_SHARED, d ##var, 0); \
  if(var == (u32)MAP_FAILED) { PRINTK("## mmap failed \"" name "\" 0x%x size %d : %s\n",add,size,strerror(errno));exit(1);} \
  DBG_KERN("* " name " mapped at 0x%x\n",var);\
  
#define HMAP_DEV_FIX(var,add,size,name) 	\
  int d##var ;\
  d ##var=open(name, O_RDWR | O_SYNC);\
  if(d ##var<0){PRINTK("## cannot open " name "\n");exit(1);}\
  var =  (u32)mmap((void*) add ,size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED|MAP_LOCKED, d ##var, 0); \
  if(var == (u32)MAP_FAILED) { PRINTK("## mmap failed \"" name "\" 0x%x size %d   : %s\n",add,size,strerror(errno));exit(1);} \
  DBG_KERN("* " name " mapped at 0x%x\n",var);\

#define HMAP_PDEV(var,add,size,name) {	\
  int d##var ;\
  d ##var=open(name, O_RDWR | O_SYNC);\
  if(d ##var<0){PRINTK("## cannot open " name "\n");exit(1);}\
  var =  (u32)mmap(0,size, PROT_READ | PROT_WRITE, MAP_SHARED, d ##var, add); \
  if(var == (u32)MAP_FAILED) { PRINTK("## mmap phys device failed \"" name "\" 0x%x size %d    : %s\n",add,size,strerror(errno));exit(1);} \
  DBG_KERN("* " name " mapped at 0x%x\n",var);\
  }
#endif

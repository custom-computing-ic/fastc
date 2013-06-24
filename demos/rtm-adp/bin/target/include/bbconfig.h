/**
 * @author       Andrea Michelotti
 *               BlueBee <br>
 *               andrea.michelotti@gmail.com
 *               http://www.michelotti.eu
 *
 * @copyright    Copyright (c)  2010 by BlueBee.<br>
 *               All rights reserved.<br>
 *               BlueBee provides this product without warranty of
 *               any kind and shall not be liable for any damages caused by
 *               the use of this product.
 *
 *
 */

#ifndef __BBCONFIG_H__
#define __BBCONFIG_H__ 
#include <stdint.h>

/**
   \mainpage BB Runtime
   * <b>Introduction</b>
   * <b>Copyright Notice</b>
   *
   * Copyright (C) 2010 BlueBee Limited. All rights reserved.

   
*/

/**
 * @defgroup RuntimeInterface BB Runtime Interface
 * Structures and APIs to comunicate with the BB RT 
 */


/**
  Runtime and Kernel default configuration
*/
#define MAX_REMOTE_ARGUMENTS_SPACE 42
#define DEFAULT_UKNOWN_POINTER_SIZE 256
#define HMAX_CACHE_SIZE 4*1024
#define MAX_CTX_SCRATCH_AREA 256
#define MAX_POINTER_TRACE 256
/*
  function/threads are started by using shared memory,
  thread information are kept in the shared memory

 */
#define HCONTROL_STRUCTURE_SHARED 1

#define SHDEFAULT 0


#ifdef LINUX
#define HFAST_MUTEX_LOCK(var)  pthread_mutex_lock(&(var));
#define HFAST_MUTEX_UNLOCK(var) pthread_mutex_unlock(&(var));
#else
#define HFAST_MUTEX_LOCK(var)  
#define HFAST_MUTEX_UNLOCK(var)
#endif

#endif

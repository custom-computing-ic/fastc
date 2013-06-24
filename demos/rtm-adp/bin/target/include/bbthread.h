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

#ifndef __BBTHREAD_H__
#define __BBTHREAD_H__
#include "bbtypes.h"
//#include "hsched.h"
#include "bbsignum.h"

typedef uint32_t bb_thread_t;
typedef uint32_t bb_sigset_t;


enum {
  BB_MUTEX0,
  BB_MUTEX1,
  BB_MUTEX2,
  BB_MUTEX3,
  BB_MUTEX4,
  BB_MUTEX5,
  BB_MUTEX6,
  BB_MUTEX7,
  BB_MUTEX8,
  BB_MUTEX9,
  BB_MUTEX10,
  BB_MUTEX11,
  BB_MUTEX12,
  BB_MUTEX13,
  BB_MUTEX14
};




typedef uint32_t bb_thread_mutex_t;

typedef struct _hthread_attr {
  int32_t stack_size;
  int32_t prio;
} bb_thread_attr_t;

/**
\brief creates a Posix hartes thread, on the specified PE (0 is the GPP). 
\param [in] peid : PE identifier
\param [out] thread_p : return the handle of the thread
\param [in] attr : handle to the bb_thread_attr_t structure
\param [in] start_routine address of PE routine
\param [in] arg: 32 bit argument to be passed to the start_rtoutine
\return 0 on success
*/
int bb_thread_create(int peid,bb_thread_t * thread_p, bb_thread_attr_t * attr, void * (*start_routine)(void *), void * arg);

/**
\brief Posix hartes thread join
\param[in] th hartes thread handle
\param[out] thread_return return value
\return 0 on success
*/
int bb_thread_join(bb_thread_t th, void **thread_return);

/**
\brief Initalize hArtes thread attributes
\param[in] attr hartes thread attributes handle
\return Upon successful completion, shall return  a  value  of  0
*/
int bb_thread_attr_init(bb_thread_attr_t *attr);
/**
   \brief bb_thread_attr_getstacksize  shall get the thread creation stacksize attribute in the attr object.
 */
int bb_thread_attr_getstacksize(const bb_thread_attr_t *attr,hsize_t *stacksize);
/**
   \brief bb_thread_attr_setstacksize  shall set the thread creation stacksize attribute in the attr object.
 */
int bb_thread_attr_setstacksize(bb_thread_attr_t *attr, hsize_t stacksize);



/**
\brief hartes thread status
\param[in] th hartes thread to query
\return thread status (now is the status of magic)
*/
int bb_thread_get_status(bb_thread_t th);

/**
   \brief list active threads
 */
void bb_thread_list();

/**
   \brief get running
   \return 0 if no threads are running
*/

int bb_thread_kill(bb_thread_t thread, int signo);


int hsigwait(const hsigset_t *set, int *sig);
int bb_thread_mutex_unlock(bb_thread_mutex_t mutex);
int bb_thread_mutex_trylock(bb_thread_mutex_t mutex);
int bb_thread_mutex_lock(bb_thread_mutex_t mutex);
bb_thread_t bb_thread_self();




/*!
 */

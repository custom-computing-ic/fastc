/**
 * @author       Andrea Michelotti
 *               BlueBee <br>
 *               andrea.michelotti@bluebee-tech.com
 *               http://www.bluebee-tech.com, http://www.michelotti.eu
 *
 * @copyright    Copyright (c)  2010 BlueBee.<br>
 *               All rights reserved.<br>
 *               BlueBee provides this product without warranty of
 *               any kind and shall not be liable for any damages caused by
 *               the use of this product.
 *
 *
 */

#ifndef __BBERRORS_H__
#define __BBERRORS_H__

#define BBNOERR 0
#define BBINTERNALERR 1
#define BBBAD_PARAMETERS 2
#define BBTHREAD_NOT_SUSPENDED 3
#define BBTHREAD_THREAD_NOT_MUTEX_OWNER 4
#define BBTHREAD_THREAD_NOT_IN_WAIT_STATUS 5
#define BBMUTEX_WRONG_MUTEX 6
#define BBCANNOT_ALLOCATE_MEMORY 7
#define BBGENERAL_APPLICATION_ERROR 8
#define BBMEMORY_CHECK_ERROR 9
#define BBNUMERRORS 10

#define BBEXIT_ERROR(x) (BBNUMERRORS + x)

#define BBNOERR_STR "no error"
#define BBINTERNALERR_STR "internal error"
#define BBBAD_PARAMETERS_STR "bad parameters"
#define BBTHREAD_NOT_SUSPENDED_STR "resuming thread not in suspended state"
#define BBTHREAD_THREAD_NOT_MUTEX_OWNER_STR "thread cannot unlock mutex: NOT OWNER"
#define BBTHREAD_THREAD_NOT_IN_WAIT_STATUS_STR "thread not in wait status"
#define BBMUTEX_WRONG_MUTEX_STR "wrong mutex number (must be between 0-15)" 
#define BBCANNOT_ALLOCATE_MEMORY_STR "cannot allocate memory"
#define BBGENERAL_APPLICATION_ERROR_STR "application error"
#define BBMEMORY_CHECK_ERROR_STR "memory check error"

void bberror(int err);

#endif

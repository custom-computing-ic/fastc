/**
 * @author       Andrea Michelotti
 *          BlueBee     
 *               andrea.michelotti@gmail.com
 *               http://www.michelotti.eu
 *
 * @copyright    
 *               
 *               
 *               
 *               
 *
 *
 */

#ifndef __BBTIMERS_H__
#define __BBTIMERS_H__

#include "bbtypes.h"
/**
   \headerfile bbtimers.h "bbtimers.h"
   \ingroup RuntimeInterface
*/

/**
 *   \defgroup BBTIMAPI BB Timers APIs
 *   \brief This APIs are used by profiling. 
 *   \addtogroup BBTIMAPI
 *  @{
 */
/**
   converts ticks into NS
 */
#define TICK_TO_NS(x) (((float)(x*1000.0))/bb_get_timer_freq())

/**
   converts ticks into Seconds
 */
#define TICK_TO_SEC(x) (((float)(x))/(1000000.0*bb_get_timer_freq()))
/**
   converts ticks into milli seconds
 */
#define TICK_TO_MSEC(x) (((float)(x))/(1000.0*bb_get_timer_freq()))
/**
   converts ticks into micro seconds
 */
#define TICK_TO_USEC(x) (((float)(x))/(1.0*bb_get_timer_freq()))
/**
   converts NS into ticks
 */
#define NS_TO_TICK(x) (bb_get_timer_freq()*((x)/1000))


/**
   \brief returns the actual value of the platform timer tick counter
   \return return the actual value in ticks of the free running tick 
 */
uint64_t bb_get_tick();

/**
   \brief returns the actual value of the timer clock
   \return returns the timer clock frequency in MHz
 */
float bb_get_timer_freq();

/**
   \brief  suspends  the  execution  of  the  calling  thread until the  time specified expires
   \return return 0 on success, -1 on failure
 */
int bb_nanoleep(uint32_t);

#ifdef PROFILE
#define START_TIME(x) (x)= bb_get_tick() 
#define END_TIME(x) (x)= bb_get_tick() - (x)
#else
#define START_TIME(x)
#define END_TIME(x)
#endif
/** @} */
/*!
*/
/*!
*/
#endif

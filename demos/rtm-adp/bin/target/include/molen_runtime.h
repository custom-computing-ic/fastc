#ifndef _MOLEN_RUNTIME_H_
#define _MOLEN_RUNTIME_H_
#include "bbtypes.h"
#include "bbconfig.h"
#include "hrpc_types.h"
#include "bbtimers.h"

/**
   \headerfile molen_runtime.h "molen_runtime.h"
   \ingroup RuntimeInterface
  
*/

/** 
 * \defgroup MOLEGENRT Molengen Interface
 * \brief This module interface <b>molengen</b> runtime generation with BB RT
 * \addtogroup MOLEGENRT
 *  @{
 */


#define PE_CTRL_NONE   0x0 
/**
  \brief stop  a running PE
*/
#define PE_CTRL_STOP   0x1 
/** 
  \brief start  a PE
*/
#define PE_CTRL_START  0x2 
/** 
  \brief step a running PE  (debug purposes)
*/
#define PE_CTRL_STEP   0x4 
/** 
  \brief pauses a running PE (debug purposes) 
*/
#define PE_CTRL_PAUSE  0x8 
/** 
  \brief resumes a paused PE (debug purposes)
*/
#define PE_CTRL_CONT   0x10 
/** 
  \brief refreshes status 
 */
#define PE_CTRL_UPDATE 0x20 

/** 
  \brief PE is idle
 */
#define PE_STATUS_IDLE 0x0
/** 
  \brief PE is running
 */
#define PE_STATUS_RUN  0x1 
/** 
  \brief PE exited after run
 */
#define PE_STATUS_EXIT 0x2 
/** 
  \brief PE arised exception
 */
#define PE_STATUS_EXC  0x4 

/** 
  \brief PE is waiting for a service from GPP
 */
#define PE_STATUS_WAIT 0x8
/** 
  \brief PE paused
 */
#define PE_STATUS_PAUSE 0x10
/** 
  \brief PE is in step mode
 */
#define PE_STATUS_STEPMODE  0x20 
/** 
  \brief PE is in debug
 */
#define PE_STATUS_DEBUGMODE  0x40 

/**
   \brief PE is signalling a service request
 */
#define PE_SIG_SERVICE 0x1

/**
   \typedef pe_ctx_t
   \brief generic PE context
   This structure represents the interface with the PE\n
   It's used to control and check the PE
 */
typedef struct pe_ctx {
  /**
    PE's control register
  */
  uint32_t ctrl; /* write */
  /**
    PE's status register
  */
  uint32_t status; /* read */
  
  /**
    stack pointer, pointer to an area to save restore registers
  */
  uint32_t sp; 
  /**
    program counter, identifies the routine to call, it can be updated by the PE for debugging purposes
  */
  uint32_t pc; 
  /**
     tick counter
     incrementing when in run
  */
  uint32_t tick;
  /**
     signals collected by PE
  */
  uint32_t signals; 
  /**
     PE mutex, for interlocking
  */
  uint32_t mutex; 
  /** 
     function arguments 
  */
  uint32_t args[MAX_REMOTE_ARGUMENTS_SPACE]; 

} pe_ctx_t;

struct profileCall;
struct profileImpl;

/**
   \typedef bb_impl_t
   \brief This structure represents the function that has been created for a gicen PE type. In the profile mode, this structure holds profiling parameters (like times) that characterize the implementation

 */
typedef struct _bb_impl_t {
   int pe_type;
  /**
     identifies uniquily a function inside a PE type 
   */
   uint32_t implementation_id; 

#ifdef BBPROFILE
  struct profileImpl*prof;
#endif
} bb_impl_t;

typedef struct _bb_func_t {
   bb_impl_t *implementations;
   int n_implem;

} bb_func_t;


/**
   \typedef bb_call_t
   \brief this represent a remote call.
 */
typedef struct _bb_call_t {
  /* the index in the function array */
  int func_idx;
  /* the index into the implementation array (the actual mapping decided by hArmonic) */
  int impl_idx;
  /* which of the PE-s instances will be used  */
  int pe_id;
  /* current implementation, to fast access */
  bb_impl_t*implementation;

#ifdef BBPROFILE
  /** number of args */
  int args;
  struct profileCall*prof;
#endif
#if defined(BBKERNDEBUG) || defined(BBPROFILE)
  const char* calln;
  const char* fname;
  int line;
  int col;
#endif
} bb_call_t;
/**
  \brief remote calls descriptors
 */
extern bb_call_t _bb_remote_calls[];
extern const bb_func_t _bb_functions[];
/**
   \brief Runtime Initialization routine, called at startup
 */
extern void _bbInitRuntime();
/**
   \brief Number of remote calls
 */
extern int _bb_remote_calls_n;

#define molen_error_report() bbhprintf("PE error\n");
#endif 

#ifndef __HRPC_TYPE_H__
#define __HRPC_TYPE_H__
/**
   \headerfile hrpc_types.h "hrpc_types.h"
   \ingroup RuntimeInterface
*/

/** 
 *  \brief BB RPC types and attributes
 *    \addtogroup MOLEGENRT
 *  @{
 */
#define HRPC_TYPE_PARAM_MASK     0x0000000F
/** 
    \brief unspecified 32 bit data
 */
#define HRPC_TYPE_RAW32          0x00000000
/** 
    \brief integer 8 bit data
 */
#define HRPC_TYPE_INT8           0x00000001
/** 
    \brief integer 16 bit data
 */
#define HRPC_TYPE_INT16          0x00000002
/** 
    \brief integer 32 bit data
 */
#define HRPC_TYPE_INT32          0x00000003
/** 
    \brief integer 64 bit data
 */
#define HRPC_TYPE_INT64          0x00000004
/** 
    \brief integer 128 bit data
 */
#define HRPC_TYPE_INT128         0x00000005
/** 
    \brief Floating Point 32 IEEE data
 */
#define HRPC_TYPE_FLOAT32        0x00000006
/** 
    \brief Floating Point 64 IEEE data
 */
#define HRPC_TYPE_DOUBLE32       0x00000007

/** 
    \brief Integer type is unsigned data
 */
#define HRPC_TYPE_UNSIGNED       0x00000010




#define HRPC_NATURE_MASK         0x0000FF00
/** 
    \brief a value is passed
 */
#define HRPC_NATURE_VALUE        0x00000000
/** 
    \brief an array is passed
 */
#define HRPC_NATURE_ARRAY        0x00000100
/** 
    \brief a pointer is passed
 */
#define HRPC_NATURE_POINTER      0x00000200
/** 
    \brief a multidimentional array is passed
 */
#define HRPC_NATURE_MULTI        0x00000400
/** 
    \brief a vector is passed
 */
#define HRPC_NATURE_VECTOR       0x00000800
/** 
    \brief a record is passed
 */
#define HRPC_NATURE_RECORD       0x00001000


#define HRPC_ATTRIBUTE_MASK      0x00FF0000

/** 
   \brief read/write attribute 
 */
#define HRPC_ATTRIBUTE_RW        0x00000000
/** 
   \brief read only attribute
   \brief the pointed area is never written by the PE
 */
#define HRPC_ATTRIBUTE_RO        0x00010000
/** 
   \brief write only attribute 
   \brief the pointed area is only written by the PE
 */
#define HRPC_ATTRIBUTE_WO        0x00020000

/** 
   \brief const pointer/value attribute
   \brief the pointed area is constant, never written (neither PEs nor GPP), just initialized a loading time (data section)
 */
#define HRPC_ATTRIBUTE_CONST     0x00040000

/**
   \brief return pointer (WO) attribute
   \brief to be used to tag a return pointer
 */
#define HRPC_ATTRIBUTE_RETURN    0x00080000

/** 
   \brief the pointer area must be cached by the PE,
   \brief if no RO and or WO attribute is set, the content is not syncronized with GPP 
   HRPC_ATTRIBUTE_RO + HRPC_ATTRIBUTE_CACHE : memory area is synchronized (invalidated) in input
   HRPC_ATTRIBUTE_WO + HRPC_ATTRIBUTE_CACHE : memory area is synchronized (invalidated) in output
   HRPC_ATTRIBUTE_RO + HRPC_ATTRIBUTE_WO + HRPC_ATTRIBUTE_CACHE : memory area is synchronized (invalidated) in input and output (but not deallocated)
   HRPC_ATTRIBUTE_CACHE : memory area is not deallocated (if not required) and not synchronized with GPP

*/
#define HRPC_ATTRIBUTE_CACHE     0x00100000
/**
  \brief the pointer area, is allocated by the application
  \brief this are will not freed by the RT
 */
#define HRPC_ATTRIBUTE_STATIC    0x00200000

/**
   \brief pointed area has already been specified in another argument
   \brief pointed area has already been specified in another argument
*/
#define HRPC_ATTRIBUTE_ALIAS     0x00400000 

/** @} */

#endif

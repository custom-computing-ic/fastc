/**
 * @author       Andrea Michelotti
 *               BlueBee <br>
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

#ifndef __BBSTDLIB_H__
#define __BBSTDLIB_H__
#include "bbtypes.h"

/**
   \headerfile bbstdlib.h "bbstdlib.h"
   \ingroup RuntimeInterface
*/

/** 
 *   \defgroup BBMEMAPI BB Memory APIs
 *   \brief BB APIs for memory management. These APIs are used by partition and mapping tools in order to substitute traditional LIBC dynamic allocations.
 *   \addtogroup BBMEMAPI
 *  @{
 */

/**
   \brief allocate size bytes into the specified memory areas
   \param[in] size: size in bytes to be allocated  
   \param[in] allocatorID: allocator ID
   \return the address of the allocated memory, 0 if fails
 */
uint32_t bb_malloc_f(uint32_t size,uint32_t allocatorID);

/**
   \brief free preallocated memory pointer
   \param[in] pointer: preallocated pointer
 */
void bb_free(uint32_t pointer);

/**
   \brief Allocates size bytes into the default shared memory (allocatorID =0). 
   \param[in] size: size in bytes to be allocated (must be multiple of GPP align size) 
   \return the address of the allocated memory, 0 if fails
 */
uint32_t bb_malloc(uint32_t size);

/**
   \brief In the GPP allocates size bytes into the default shared memory for elem of size size and initialize it to zero. For PE, depends on implementation (internal memory is the default).
   \param[in] nelem: number of elements
   \param[in] size: size in bytes of the element (must be multiple of GPP align size) 
   \return the address of the allocated memory, 0 if fails
 */
uint32_t bb_calloc(uint32_t nelem,uint32_t size);


/**
   \brief re-allocate size bytes, if size =0 then free, if pointer=0 then is equivalent to a bb_malloc
   \param[in] pointer: preallocated pointer
   \param[in] size: size in bytes to be allocated (must be multiple of GPP align size)  
   \return a new pointer, 0 if fails
 */
uint32_t bb_realloc(uint32_t pointer,uint32_t size);

/**
   \brief return the free space
   \param[in] memarea \ref memoryareas
   \return the free space in bytes
 */
uint32_t bb_heap_size(uint32_t allocatorID);


/**
   \brief return information about a pointer allocated by bb_malloc, 
   \param[in] pointer: pointer to a pre-allocated memory area (allocated with a bb_malloc_f or bb_malloc)
   \param[out] base: pointer returned by (bb_malloc_f or bb_malloc)
   \param[out] size: size in bytes from the pointer to the end of allocation
   \return -1 if informations cannot be retrived or the allocatorID (>=0) if success
 */
inline int32_t bb_query_pointer(uint32_t pointer,uint32_t*base,uint32_t*size);

/** @} */
/**

*/
#endif

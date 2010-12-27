#ifndef PFUNC_ATOMICS_HPP
#define PFUNC_ATOMICS_HPP

/**
 * \file pfunc_atomics.h
 * \brief Implements atomic operations on PODs.
 * \author Prabhanjan Kambadur
 *
 */

#include <pfunc/config.h>
#include <pfunc/environ.hpp>

#if defined (c_plusplus) || defined (__cplusplus)
extern "C" {
#endif

#if PFUNC_HAVE_STDINT_H == 1
#include <stdint.h>
#else
typedef _int32 int32_t;
typedef _int16 int16_t;
typedef _int8 int8_t;
#endif

/**
 * \brief Compare and Swap an 8-bit memory location
 *
 * \param dest Pointer to the memory location of the operand
 * \param exchg The new value to which dest has to be set if successful
 * \param comprnd The value that is compared to the memory location
 *
 * \code 
 * if (*dest == comprnd) {
 *   *dest = exchg;
 *   return exchg;
 * } else {
 *   return *dest;
 * }  
 * \endcode
 */ 
static PFUNC_INLINE int8_t pfunc_compare_and_swap_8 
              (volatile void* dest, int8_t exchg, int8_t comprnd); 

/**
 * \brief Compare and Swap an 16-bit memory location
 *
 * \param dest Pointer to the memory location of the operand
 * \param exchg The new value to which dest has to be set if successful
 * \param comprnd The value that is compared to the memory location
 *
 * \code 
 * if (*dest == comprnd) {
 *   *dest = exchg;
 *   return exchg;
 * } else {
 *   return *dest;
 * }  
 * \endcode
 */ 
static PFUNC_INLINE int16_t pfunc_compare_and_swap_16 
                   (volatile void* dest, int16_t exchg, int16_t comprnd); 

/**
 * \brief Compare and Swap an 32-bit memory location
 *
 * \param dest Pointer to the memory location of the operand
 * \param exchg The new value to which dest has to be set if successful
 * \param comprnd The value that is compared to the memory location
 *
 * \code 
 * if (*dest == comprnd) {
 *   *dest = exchg;
 *   return exchg;
 * } else {
 *   return *dest;
 * }  
 * \endcode
 */ 
static PFUNC_INLINE int32_t pfunc_compare_and_swap_32 
              (volatile void* dest, int32_t exchg, int32_t comprnd); 

/**
 * \brief Atomically fetches 8-bit value from memory and stores a new value
 *
 * \param location Pointer to memory location
 * \param new_val The new value to be atomically written in
 *
 * \code
 * result = *location;
 * *location = new_val;
 * return result;
 * \endcode
 */ 
static PFUNC_INLINE int8_t pfunc_fetch_and_store_8 
                    (volatile void* location, int8_t new_val); 

/**
 * \brief Atomically fetches 16-bit value from memory and stores a new value
 *
 * \param location Pointer to memory location
 * \param new_val The new value to be atomically written in
 *
 * \code
 * result = *location;
 * *location = new_val;
 * return result;
 * \endcode
 */ 
static PFUNC_INLINE int16_t pfunc_fetch_and_store_16 
                      (volatile void* location, int16_t new_val); 

/**
 * \brief Atomically fetches 32-bit value from memory and stores a new value
 *
 * \param location Pointer to memory location
 * \param new_val The new value to be atomically written in
 *
 * \code
 * result = *location;
 * *location = new_val;
 * return result;
 * \endcode
 */ 
static PFUNC_INLINE int32_t pfunc_fetch_and_store_32 
                      (volatile void* location, int32_t new_val); 

/**
 * \brief Atomically fetches 8-bit value from memory and adds to it.
 *
 * \param location Pointer to memory location
 * \param addend The increment to the memory location. 
 *
 * \code
 * result = *location;
 * *location += addend;
 * return result;
 * \endcode
 */ 
static PFUNC_INLINE int8_t pfunc_fetch_and_add_8 
                  (volatile void* location, int8_t addend); 

/**
 * \brief Atomically fetches 16-bit value from memory and adds to it.
 *
 * \param location Pointer to memory location
 * \param addend The increment to the memory location. 
 *
 * \code
 * result = *location;
 * *location += addend;
 * return result;
 * \endcode
 */ 
static PFUNC_INLINE int16_t pfunc_fetch_and_add_16 
                    (volatile void* location, int16_t addend); 

/**
 * \brief Atomically fetches 32-bit value from memory and adds to it.
 *
 * \param location Pointer to memory location
 * \param addend The increment to the memory location. 
 *
 * \code
 * result = *location;
 * *location += addend;
 * return result;
 * \endcode
 */ 
static PFUNC_INLINE int32_t pfunc_fetch_and_add_32 
                    (volatile void* location, int32_t addend); 

/**
 * \brief Read an 8-bit value and insert a fence after the read
 * 
 * \param location The memory from which to read
 * \return Value at location
 */ 
static PFUNC_INLINE int8_t pfunc_read_with_fence_8 (volatile void* location); 

/**
 * \brief Read an 16-bit value and insert a fence after the read
 * 
 * \param location The memory from which to read
 * \return Value at location
 */ 
static PFUNC_INLINE int16_t pfunc_read_with_fence_16 (volatile void* location); 

/**
 * \brief Read an 32-bit value and insert a fence after the read
 * 
 * \param location The memory from which to read
 * \return Value at location
 */ 
static PFUNC_INLINE int32_t pfunc_read_with_fence_32 (volatile void* location); 

/**
 * \brief Write an 8-bit value and insert a fence before the write.
 * 
 * \param location The memory to write to
 * \param value The value to write
 */ 
static PFUNC_INLINE void pfunc_write_with_fence_8 
                (volatile void* location, int8_t value); 

/**
 * \brief Write an 16-bit value and insert a fence before the write.
 * 
 * \param location The memory to write to
 * \param value The value to write
 */ 
static PFUNC_INLINE void pfunc_write_with_fence_16 
                (volatile void* location, int16_t value); 

/**
 * \brief Write an 32-bit value and insert a fence before the write.
 * 
 * \param location The memory to write to
 * \param value The value to write
 */ 
static PFUNC_INLINE void pfunc_write_with_fence_32 
                  (volatile void* location, int32_t value);

#if defined (c_plusplus) || defined (__cplusplus)
}
#endif

/********************************************************
 * BEGIN: Cast operators
 *******************************************************/
#if defined (c_plusplus) || defined (__cplusplus)

#define PFUNC_STATIC_CAST(T, expr) static_cast<T>(expr)
#define PFUNC_CONST_CAST(T, expr) const_cast<T>(expr)
#define PFUNC_DYNAMIC_CAST(T, expr) dynamic_cast<T>(expr)
#define PFUNC_REINTERPRET_CAST(T, expr) reinterpret_cast<T>(expr)

#else /* use the regular C-style cast */

#define PFUNC_STATIC_CAST(T, expr) (T)(expr)
#define PFUNC_CONST_CAST(T, expr) (T)(expr)
#define PFUNC_DYNAMIC_CAST(T, expr) (T)(expr)
#define PFUNC_REINTERPRET_CAST(T, expr) (T)(expr)

#endif
/********************************************************
 * END: Cast operators
 *******************************************************/

/** Now, include the right file based on the environment */
#if PFUNC_WINDOWS == 1 
#include <pfunc/asm/win.h>
#elif PFUNC_X86 == 1
#include <pfunc/asm/x86.h>
#elif PFUNC_PPC == 1 && PFUNC_HAVE_PTHREADS == 1
#include <pfunc/asm/ppc.h>
#include <pfunc/asm/generic.h>
#elif PFUNC_HAVE_PTHREADS == 1
#include <pfunc/asm/generic.h>
#endif

#endif /** PFUNC_ATOMICS_HPP */

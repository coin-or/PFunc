#ifndef PFUNC_GENERIC_H
#define PFUNC_GENERIC_H

# include <pthread.h>

#define pfunc_compare_and_swap_generic(size_in_bits) \
  static pthread_mutex_t pfunc_compare_and_swap_##size_in_bits##_lock = PTHREAD_MUTEX_INITIALIZER; \
  int##size_in_bits##_t result; \
  volatile int##size_in_bits##_t* cast_loc = PFUNC_STATIC_CAST(volatile int##size_in_bits##_t*,location); \
  pthread_mutex_lock (&pfunc_compare_and_swap_##size_in_bits##_lock); \
  if (*cast_loc == comprnd) {  \
    *cast_loc = exchg; \
    result = exchg; \
  } else { \
    result = *cast_loc; \
  } \
  pthread_mutex_unlock (&pfunc_compare_and_swap_##size_in_bits##_lock); \
  return result; 

#define pfunc_fetch_and_store_generic(size_in_bits) \
  static pthread_mutex_t pfunc_fetch_and_store_##size_in_bits##_lock = PTHREAD_MUTEX_INITIALIZER; \
  int##size_in_bits##_t result; \
  volatile int##size_in_bits##_t* cast_loc = PFUNC_STATIC_CAST(volatile int##size_in_bits##_t*,location); \
  pthread_mutex_lock (&pfunc_fetch_and_store_##size_in_bits##_lock); \
  result = *cast_loc; \
  *cast_loc = new_val; \
  pthread_mutex_unlock (&pfunc_fetch_and_store_##size_in_bits##_lock); \
  pfunc_mem_fence(); \
  return result;

#define pfunc_fetch_and_add_generic(size_in_bits) \
  static pthread_mutex_t pfunc_fetch_and_add_##size_in_bits##_lock = PTHREAD_MUTEX_INITIALIZER; \
  int##size_in_bits##_t result; \
  volatile int##size_in_bits##_t* cast_loc = PFUNC_STATIC_CAST(volatile int##size_in_bits##_t*,location); \
  pthread_mutex_lock (&pfunc_fetch_and_add_##size_in_bits##_lock); \
  result = *cast_loc; \
  *cast_loc += addend; \
  pthread_mutex_unlock (&pfunc_fetch_and_add_##size_in_bits##_lock); \
  pfunc_mem_fence(); \
  return result;

int8_t pfunc_compare_and_swap_8 (volatile void* location,
                                 int8_t exchg, 
                                 int8_t comprnd) {
  pfunc_compare_and_swap_generic(8)
}

int16_t pfunc_compare_and_swap_16 (volatile void* location, 
                                   int16_t exchg, 
                                   int16_t comprnd) {
  pfunc_compare_and_swap_generic(16)
}

int32_t pfunc_compare_and_swap_32 (volatile void* location, 
                                   int32_t exchg, 
                                   int32_t comprnd) {
  pfunc_compare_and_swap_generic(32)
}

int8_t pfunc_fetch_and_store_8 (volatile void* location, 
                                int8_t new_val) {
  pfunc_fetch_and_store_generic(8)
}

int16_t pfunc_fetch_and_store_16 (volatile void* location, 
                                  int16_t new_val) {
  pfunc_fetch_and_store_generic(16)
}

int32_t pfunc_fetch_and_store_32 (volatile void* location, 
                                  int32_t new_val) {
  pfunc_fetch_and_store_generic(32)
}

int8_t pfunc_fetch_and_add_8 (volatile void* location, 
                              int8_t addend) {
  pfunc_fetch_and_add_generic(8)
}

int16_t pfunc_fetch_and_add_16 (volatile void* location, 
                                int16_t addend) {
  pfunc_fetch_and_add_generic(16)
}

int32_t pfunc_fetch_and_add_32 (volatile void* location, 
                                int32_t addend) {
  pfunc_fetch_and_add_generic(32)
}

#endif /** PFUNC_GENERIC_H */

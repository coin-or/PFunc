#ifndef PFUNC_POWERPC_H
#define PFUNC_POWERPC_H

#define pfunc_mem_fence() __asm__ __volatile__ ("sync")

int8_t pfunc_read_with_fence_8 (volatile void* location) {
  int8_t const result = *(PFUNC_STATIC_CAST(volatile int8_t*,location));
  pfunc_mem_fence();
  return result;
}

int16_t pfunc_read_with_fence_16 (volatile void* location) {
  int16_t const result = *(PFUNC_STATIC_CAST(volatile int16_t*,location));
  pfunc_mem_fence();
  return result;
}

int32_t pfunc_read_with_fence_32 (volatile void* location) {
  int32_t const result = *(PFUNC_STATIC_CAST(volatile int32_t*,location));
  pfunc_mem_fence();
  return result;
}

void pfunc_write_with_fence_8 (volatile void* location, 
                                      int8_t value) {
  pfunc_mem_fence();
  *(PFUNC_STATIC_CAST(volatile int8_t*,location)) = value;
}

void pfunc_write_with_fence_16 (volatile void* location, 
                                       int16_t value) {
  pfunc_mem_fence();
  *(PFUNC_STATIC_CAST(volatile int16_t*,location)) = value;
}

void pfunc_write_with_fence_32 (volatile void* location, 
                                       int32_t value) {
  pfunc_mem_fence();
  *(PFUNC_STATIC_CAST(volatile int32_t*,location)) = value;
}

#endif /** PFUNC_POWERPC_H */

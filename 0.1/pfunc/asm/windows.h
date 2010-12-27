#ifndef PFUNC_WINDOWS_H
#define PFUNC_WINDOWS_H

#define pfunc_mem_fence() MemoryBarrier()

int8_t pfunc_read_with_fence_8 (volatile void* location) {
  int8_t const result = *(PFUNC_STATIC_CAST(int8_t*,location));
  pfunc_mem_fence();
  return result;
}

int16_t pfunc_read_with_fence_16 (volatile void* location) {
  int16_t const result = *(PFUNC_STATIC_CAST(int16_t*,location));
  pfunc_mem_fence();
  return result;
}

int32_t pfunc_read_with_fence_32 (volatile void* location) {
  int32_t const result = *(PFUNC_STATIC_CAST(int32_t*,location));
  pfunc_mem_fence();
  return result;
}

void pfunc_write_with_fence_8 (volatile void* location, 
                               int8_t value) {
  pfunc_mem_fence();
  *(PFUNC_STATIC_CAST(int8_t*,location)) = value;
}

void pfunc_write_with_fence_16 (volatile void* location, 
                                       int16_t value) {
  pfunc_mem_fence();
  *(PFUNC_STATIC_CAST(int16_t*,location)) = value;
}

void pfunc_write_with_fence_32 (volatile void* location, 
                                       int32_t value) {
  pfunc_mem_fence();
  *(PFUNC_STATIC_CAST(int32_t*,location)) = value;
}

int8_t pfunc_compare_and_swap_8 (volatile void* dest, 
                                 int8_t exchg, 
                                 int8_t comprnd) {
  int8_t result;
  __asm { 
    __asm mov al, comprnd
    __asm mov dl, exchg
    __asm lock cmpxchg [dest], dl
    __asm mov result, al
  }

  return result;
}

int16_t pfunc_compare_and_swap_16 (volatile void* dest, 
                                   int16_t exchg, 
                                   int16_t comprnd) {
  int16_t result;
  __asm { 
    __asm mov ax, comprnd
    __asm mov dx, exchg
    __asm lock cmpxchg [dest], dx
    __asm mov result, ax
  }

  return result;
}

int32_t pfunc_compare_and_swap_32 (volatile int32_t* dest,
                                   int32_t exchg,
                                   int32_t comprnd) {
  return InterlockedCompareExchange (PFUNC_STATIC_CAST(volatile LONG*,dest), 
                                     exchg, comprnd);
}

int8_t pfunc_fetch_and_store_8 (volatile void* location, 
                                int8_t new_val) {
  int8_t result;

  __asm {
    __asm mov al, new_val
    __asm xchg [location], al
    __asm mov result, al
  }

  pfunc_mem_fence();
  return result;
}

int16_t pfunc_fetch_and_store_16 (volatile void* location, 
                                  int16_t new_val) {
  int16_t result;

  __asm {
    __asm mov ax, new_val
    __asm xchg [location], ax
    __asm mov result, ax
  }

  pfunc_mem_fence();
  return result;
}

int32_t pfunc_fetch_and_store_32 (volatile void* location, 
                                  int32_t new_val) {
  return InterlockedExchange (PFUNC_STATIC_CAST(volatile LONG*,location), 
                              new_val);
}

int8_t pfunc_fetch_and_add_8 (volatile void* location, 
                              int8_t addend) {
  int8_t result;

  __asm {
    __asm mov al, addend
    __asm xadd [location], al
    __asm mov result, al
  }

  pfunc_mem_fence();
  return result;
}

int16_t pfunc_fetch_and_add_16 (volatile void* location, 
                                int16_t addend) {
  int16_t result;

  __asm {
    __asm mov ax, addend
    __asm xadd [location], ax
    __asm mov result, ax
  }

  pfunc_mem_fence();
  return result;
}

int32_t pfunc_fetch_and_add_32 (volatile void* location, 
                                int32_t addend) {
  return InterlockedAdd (PFUNC_STATIC_CAST(volatile LONG*,location), addend);
}

#endif /** PFUNC_WINDOWS_H */

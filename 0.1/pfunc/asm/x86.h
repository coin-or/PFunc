#ifndef PFUNC_X86_32_H
#define PFUNC_X86_32_H

#define pfunc_mem_fence() __asm__ __volatile__ ("mfence":::"memory")

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

int8_t pfunc_compare_and_swap_8 (volatile void* location, 
                                 int8_t exchg, 
                                 int8_t comprnd) {

  int8_t result;

  __asm__  __volatile ("lock\n\t cmpxchgb %3,(%2)\n" 
                       :"=a"(result) 
                       :"0" (comprnd) , "c"(location), "d"(exchg) 
                       :"memory"); 


  return result;
}

int16_t pfunc_compare_and_swap_16 (volatile void* location, 
                                   int16_t exchg, 
                                   int16_t comprnd) {
  int16_t result;
  __asm__  __volatile ("lock\n\tcmpxchgw %3,(%2)\n" 
                       :"=a"(result) 
                       :"0" (comprnd) , "c"(location), "d"(exchg) 
                       :"memory"); 

  return result;
}

int32_t pfunc_compare_and_swap_32 (volatile void* location, 
                                   int32_t exchg, 
                                   int32_t comprnd) {
  int32_t result;
  __asm__  __volatile ("lock\n\tcmpxchgl %3,(%2)\n" 
                       :"=a"(result) 
                       :"0" (comprnd) , "c"(location), "d"(exchg) 
                       :"memory"); 
  return result;
}

int8_t pfunc_fetch_and_store_8 (volatile void* location, 
                                int8_t new_val) {
  int8_t result;
  __asm__ __volatile__ ("lock\n\txchgb %2,(%1)"
                        : "=r" (result)
                        : "r" (location), "0"(new_val)
                        : "memory");
  return result;
}

int16_t pfunc_fetch_and_store_16 (volatile void* location, 
                                  int16_t new_val) {
  int16_t result;
  __asm__ __volatile__ ("lock\n\txchgw %2,(%1)"
                        : "=r" (result)
                        : "r" (location), "0"(new_val)
                        : "memory");
  return result;
}

int32_t pfunc_fetch_and_store_32 (volatile void* location, 
                                  int32_t new_val) {
  int32_t result;
  __asm__ __volatile__ ("lock\n\txchgl %2,(%1)"
                        : "=r" (result)
                        : "r" (location), "0"(new_val)
                        : "memory");
  return result;
}

int8_t pfunc_fetch_and_add_8 (volatile void* location, 
                              int8_t addend) {
  int8_t result;
  __asm__ __volatile__ ("lock\n\txaddb %2,(%1)"
                        : "=r" (result)
                        : "r" (PFUNC_STATIC_CAST(int8_t*,PFUNC_CONST_CAST(void*,location))), "0"(addend)
                        : "memory");
  return result;
}

int16_t pfunc_fetch_and_add_16 (volatile void* location, 
                                int16_t addend) {
  int16_t result;
  __asm__ __volatile__ ("lock\n\txaddw %2,(%1)"
                        : "=r" (result)
                        : "r" (PFUNC_STATIC_CAST(volatile int16_t*,location)), 
                          "0"(addend)
                        : "memory");
  return result;
}

int32_t pfunc_fetch_and_add_32 (volatile void* location, 
                                int32_t addend) {
  int32_t result;
  __asm__ __volatile__ ("lock\n\txaddl %2,(%1)"
                        : "=r" (result)
                        : "r" (PFUNC_STATIC_CAST(volatile int32_t*,location)), 
                          "0" (addend)
                        : "memory");
  return result;
}
          
#endif /** PFUNC_X86_32_H */

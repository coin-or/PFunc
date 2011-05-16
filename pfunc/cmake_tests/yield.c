#ifndef TEST_NAME
#error "We don't know which function we are testing for"
#endif

#if TEST_NAME == 1
#include <windows.h>
#define FUNCTION_NAME SwitchToThread
#elif TEST_NAME == 2
#include <pthread.h>
#define FUNCTION_NAME pthread_yield
#elif TEST_NAME == 3
#include <pthread.h>
#define FUNCTION_NAME pthread_yield_np
#elif TEST_NAME == 4
#include <sched.h>
#define FUNCTION_NAME sched_yield
#else 
#error "Unrecognized function to be tested"
#endif

int main () { FUNCTION_NAME (); }

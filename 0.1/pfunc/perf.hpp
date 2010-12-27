#ifndef PFUNC_PERF_T_HPP
#define PFUNC_PERF_T_HPP

#if PFUNC_USE_PAPI == 1 

#include <pfunc/config.h>
#include <papi.h>
#include <pfunc/thread.hpp>
#include <pfunc/environ.hpp>

namespace pfunc {
/**
 * \brief Virtual base class for all performance measurements
 *
 * When users want to measure certain performance parameters such as the L1 and L2
 * cache miss rates, they utilize this structure by deriving from it. There are only
 * three things that we need from this structure:
 *
 * 1. Number of events to count
 * 2. List of those events 
 * 3. The space to store all the values counted for these events
 *
 * We do not handle errors arising from insufficient memory or overflow of the
 * hardware counters.
 */
struct virtual_perf_data {
  typedef long long event_value_type;

  /** Virtual destructor */
  virtual ~virtual_perf_data () {}

  /** Get the number of events to subscribe to */
  virtual int get_num_events () const = 0;

  /** Get the list of events to subscribe to */
  virtual int* get_events () const = 0;

  /** Get the storage space for the events */
  virtual event_value_type** get_event_storage () const = 0;
};

namespace detail {
  struct perf {
    static int* event_codes;
    static int num_events;

    static bool initialize (const int* _event_codes, const int _num_events) {
      num_events = _num_events;
      event_codes = new int [num_events];

      /* Step 1: Copy the codes over */
      for (int i=0; i<num_events; ++i) event_codes[i] = _event_codes[i];

      /* Step 2: Initialize the library */
      int error;
      if (!PAPI_is_initialized ()) {
        if (PAPI_VER_CURRENT != (error = PAPI_library_init (PAPI_VER_CURRENT))) {
          printf ("PAPI_library_init failed: %d\n", error);
          return false;
        }
      }

      /* Step 3: Initialize the threading support */
      if (PAPI_OK != (error = PAPI_thread_init (pthread_self))) {
        printf ("PAPI_thread_init failed: %d\n", error);
         return false;
      }

      /* Step 4: Check if we need to multiplex */
      bool multiplex = false;
      if (num_events > PAPI_num_hwctrs ()) multiplex = true;

      /* Step 5: Check if each of the events is supported */
      for (int i=0; i<num_events; ++i)  {
        if (PAPI_OK != (error = PAPI_query_event (event_codes[i]))) {
          char event_name [1024];
          PAPI_event_code_to_name(event_codes[i], event_name);
          printf ("PAPI_query_event failed: %d (%s)\n", error, event_name);
          return false;
        }
      }
      
      /* Step 6: We have reached till here -- so if multiplex was needed, start */
      if (multiplex) {
        if (PAPI_OK != (error = PAPI_multiplex_init ())) {
          printf ("PAPI_multiplex_init failed: %d\n", error);
          return false;
        }
      }

      return true;
    }

    static void destroy () { delete [] event_codes; }

    static int create_events () {
      ALIGN64 int event_set = PAPI_NULL;

      /* Step 1: Create the event set */
      if (PAPI_OK != PAPI_create_eventset (&event_set)) return -1;

      /* Step 2: Check if this event needs to be multiplexed */
      if (num_events > PAPI_num_hwctrs ()) 
        if (PAPI_OK != PAPI_set_multiplex (event_set)) return -1;

      /* Step 3: Add all the events to this event set */
      for (int i=0; i<num_events; ++i) {
        char event_name [1024];
        PAPI_event_code_to_name(event_codes[i], event_name);
        if (PAPI_OK != PAPI_add_event (event_set, event_codes [i])) {
          printf ("PAPI could not add %s\n", event_name);
          return -1;
        } 
      }

      return event_set; 
    }

    static bool start_events (const int& event_set) {
      return (PAPI_OK == PAPI_start (event_set)) ? true : false;
    }

    static bool stop_events (const int& event_set, 
                             volatile long long int* event_values) {
      return (PAPI_OK == PAPI_stop (event_set, (long long*) event_values)) 
                                                                    ? true : false;
    }

    static int get_num_events (void) { return num_events; }

  }; // struct perf

  /**< Declare the static variables */
  int perf::num_events = 0;
  int* perf::event_codes = NULL;

} /* namespace detail */ } /* namespace pfunc */

#endif /* PFUNC_USE_PAPI */

#endif // PFUNC_PERF_T_HPP

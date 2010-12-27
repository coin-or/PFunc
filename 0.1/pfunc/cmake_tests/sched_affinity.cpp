#include <sched.h>

int main (int argc, char** argv) {
  cpu_set_t orig_set;
  cpu_set_t my_set;

  /** Get the current affinity */
  if (0 > sched_getaffinity (0, sizeof(my_set), &orig_set)) return -1;

  CPU_ZERO (&my_set);
  CPU_SET(0, &my_set);

  /** Now, set the affinity to processor 0 */
  if (0 > sched_setaffinity (0, sizeof(my_set), &my_set)) return -1;

  /** Reset the affinity to the original settings */
  if (0 > sched_setaffinity (0, sizeof(my_set), &orig_set)) return -1;

  return 0;
}

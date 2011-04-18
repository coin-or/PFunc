/***
 * Author: Prabhanjan Kambadur
 *
 * A concept that defines the type requirements for pfunc::generator.
 */
concept Generator<typename PolicyName, 
                  typename Compare, 
                  typename Work> {
  /* Associated type requirements */
  requires SchedulingPolicy<PolicyName, pfunc::detail::task<Compare, Work> >;
  requires AdaptableCallableN<Compare>;
  requires Callable0<Work>;

  /* Associate types */
  typename attribute;
  typename group;
  typename functor;
  typename task;
  typename taskmgr;
}

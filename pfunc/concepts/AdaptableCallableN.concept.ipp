/** 
 * Author: Prabhanjan Kambadur
 *
 * A concept that defines requirements for "compare" feature.
 */
concept AdaptableCallableN <typename Functor> {
  /* concept requirements */
  requires CopyConstructible <Functor> && 
           DefaultConstructible <Functor> && 
           CopyAssignable <Functor>;
  
  /* associated types */
  typename first_argument_type;
  typename result_type;
  
  /* associated type requirements */
  requires CopyConstructible <first_argument_type> && 
           DefaultConstructible <first_argument_type> && 
           CopyAssignable <first_argument_type>;
  requires CallableN <Functor, first_argument_type, ...,first_argument_type>;
  requires SameType <CallableN <Functor, first_argument_type,...>::result_type, 
                      result_type>;
}

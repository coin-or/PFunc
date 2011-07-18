/***
 * Author: Prabhanjan Kambadur
 *
 * A concept that defines the type requirements for partial order comparator.
 * This is used in prioS scheduling policy.
 */
concept PartialOrder <typename Functor> : AdaptableCallable2 <Functor> {
  /* concept requirements */
  requires SameType<result_type, bool>;

  axiom Irreflexivity (Functor& f, first_argument_type one) { 
    false == f(one,one); 
  }

  axiom AntiSymmetry (Functor& f, 
                      first_argument_type one, 
                      first_argument_type two) { 
    if (false==f(one,two)) true == f(two,one);
  }

  axiom Transitivity (Functor& f, i
                      first_argument_type one, 
                      first_argument_type two, 
                      first_argument_type three) {
    if (f(one,two) && f(two,three)) true == f(one,three);
  }
}

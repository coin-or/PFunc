/***
 * Author: Prabhanjan Kambadur
 *
 * A concept that defines the type requirements for task predicate pairs.
 */
concept TaskPredicatePair <typename PolicyName, typename PredPair> {
  /* associated types */
  typename value_type;
  typename result_type;
  
  /* associated functions */
  PredPair::PredPair(value_type);
  result_type PredPair::own_pred(value_type);
  result_type PredPair::steal_pred(value_type);
}


/** 
 * Author: Prabhanjan Kambadur
 *
 * A concept that defines requirements for policy name be a scheduling policy.
 */
concept SchedulingPolicy <typename PolicyName, typename TaskType> {
  /* concept requirements */
  requires PFuncTask <TaskType>;
  
  /* associated types */
  typename task_queue_set;
  typename regular_predicate_pair;
  typename waiting_predicate_pair;
  typename group_predicate_pair;
  
  /* associated type requirements */
  requires TaskQueueSet <PolicyName, task_queue_set>;
  requires TaskPredicatePair <PolicyName, regular_predicate_pair>;
  requires SameType <TaskType*, regular_predicate_pair::value_type>;
  requires TaskPredicatePair <PolicyName, waiting_predicate_pair>;
  requires SameType <TaskType*, waiting_predicate_pair::value_type>;
  requires TaskPredicatePair <PolicyName, group_predicate_pair>;
  requires SameType <TaskType*, group_predicate_pair::value_type>;
}

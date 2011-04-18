concept TaskQueueSet <typename PolicyName, typename QueueSet> {
  /* associated types */
  typename value_type;
  typename queue_index_type;
  
  /* associated type requirements */
  requires PFuncTask <remove_pointer<value_type>::result_type>;
  requires CopyConstructible <queue_index_type> && 
           CopyAssignable <queue_index_type> &&
           DefaultConstructible <queue_index_type>;
  
  /* associated functions */
  QueueSet::QueueSet (unsigned int);
  void QueueSet::put (queue_index_type, const value_type&);
  template <typename PredPair>
  requires TaskPredicatePair<PredPair, PolicyName> &&
           SameType<TaskPredicatePair<PredPair, PolicyName>::value_type, value_type>
  value_type QueueSet::get (queue_index_type, const PredPair&);
}

/** 
 * Author: Prabhanjan Kambadur
 *
 * A concept that defines requirements for TaskType to be a PFunc task.
 */
concept PFuncTask <typename TaskType> {
  /* concept requirements */
  requires CopyConstructible <TaskType> && CopyAssignable <TaskType>;
  
  /* associated types */
  typename compare_type;
  typename work_type;
  
  /* associated type requirements */
  requires AdaptableCallableN<compare_type>;
  requires Callable0<work_type>;
  
  /* associate functions */
  compare_type TaskType::get_compare () const;
}

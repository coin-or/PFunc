/**
 * Author: Prabhanjan Kambadur.
 * 
 * This file gives the requirements for a type T to model WhileExecutable 
 * concept. A model of this concept can be executed in paralle by 
 * pfunc::parallel_while.
 * 
 * concept WhileExecutable <typename Model, typename ArgumentType> : 
 *                                            CopyAssignable <Model> {
 *   /**< Associated types */
 *   typename argument_type;
 *
 *   /**< Associated type requirements */
 *   Model::is_convertible <argument_type, ArgumentType>;
 *
 *   /**< Associated functions */  
 *   void Model::operator() (ArgumentType) const;
 * }

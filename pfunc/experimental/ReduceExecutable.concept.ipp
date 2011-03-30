/**
 * Author: Prabhanjan Kambadur.
 *
 * This file gives the requirements for a type T to model ReduceExecutable
 * concept.  A model of this concept can be executed in parallel by
 * pfunc::parallel_reduce. 
 *
 * concept ReduceExecutable<typename Model, typename SpaceType> : 
 *              Space<SpaceType>, Assignable<Model>, CopyAssignable <Model> {
 *   /**< Associated functions */
 *   Model split () const;
 *   void join (const Model&);
 *   void operator () (const SpaceType&);
 * }
 */

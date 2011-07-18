/**
 * Author: Prabhanjan Kambadur.
 *
 * This file gives the requirements for a type T to model the Space concept.
 * Briefly, Space is a concept governs the definitions of splittable, iterable
 * points. Function execution over these points can then be parallelized by 
 * recursive splitting of the Space.
 */
concept Space<typename Model> : CopyAssignable <Model> {
  /**< Associated types */
  typename subspace_container;/**< type of the subspace container */
  requires Sequence<subspace_container>; /**< See SGI STL definition. 
                                              Required for iterability */
  
  const static size_t arity;/**< Number of ways in which a space is split */
  const static size_t dimension;/**< Dimensionality of the space */

  /**< Associated functions */
  size_t Model::begin() const;
  size_t Model::end() const;
  bool Model::can_split() const;
  subspace_container split() const;
}

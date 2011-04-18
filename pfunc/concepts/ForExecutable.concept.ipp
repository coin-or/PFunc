/**
 * Author: Prabhanjan Kambadur.
 *
 * This file gives the requirements for a type T to model ForExecutable concept.
 * A model of this concept can be executed in parallel by pfunc::parallel_for.
 */
concept ForExecutable<typename Model, typename SpaceType> : 
                           Space<SpaceType>, CopyAssignable <Model> {
  /**< Associated functions */
  void Model::operator()(const SpaceType&) const;
}


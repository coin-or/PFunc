#ifndef PFUNC_NO_COPY_HPP
#define PFUNC_NO_COPY_HPP

/**
 * \file no_copy.hpp
 * Miscellaneous things
 */ 
namespace pfunc { namespace detail {
  /**
  * no_copy
  * Ensure that all descendants of this class cannot be copied (constructor+assignment)
  */ 
  struct no_copy {
    private:
    /**
     * Copy Constructor
     * \param [in] other The object from which to copy construct
     */ 
    no_copy (const no_copy& other);

    /**
     * operator=
     * \param [in] other The object from which to copy construct
     */ 
    void operator= (const no_copy& other);

    public:
    /**
     * Constructor
     * This is the only means of constructing this object
     */
    no_copy () {} 
  };
} /* namespace detail */ } /* namespace pfunc */

#endif // PFUNC_NO_COPY_HPP

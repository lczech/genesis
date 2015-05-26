#ifndef GENESIS_###CAPMODULNAME###_###CAPFILENAME###_H_
#define GENESIS_###CAPMODULNAME###_###CAPFILENAME###_H_

/**
 * @brief
 *
 * @file
 * @ingroup ###MODULNAME###
 */

namespace genesis {

// =============================================================================
//     ###CLASSNAME###
// =============================================================================

###TEMPLATE_LINE###template <###TEMPLATE_PARAM###>
class ###CLASSNAME###
{
public:

    // -----------------------------------------------------
    //     Constructor and Destructor
    // -----------------------------------------------------

    ###CLASSNAME###();
    ~###CLASSNAME###();

};

} // namespace genesis
###TEMPLATE_START###
// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "###MODULNAME###/###FILENAME###.tpp"###TEMPLATE_END###

#endif // include guard

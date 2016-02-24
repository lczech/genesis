#ifndef GENESIS_###CAPMODULNAME###_###CAPFILENAME###_H_
#define GENESIS_###CAPMODULNAME###_###CAPFILENAME###_H_

/**
 * @brief
 *
 * @file
 * @ingroup ###MODULNAME###
 */

namespace genesis {

// =================================================================================================
//     ###CLASSNAME###
// =================================================================================================

/**
 * @brief
 */
###TEMPLATE_LINE###template <###TEMPLATE_PARAM###>
class ###CLASSNAME###
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    ###CLASSNAME###()  = default;
    ~###CLASSNAME###() = default;

    ###CLASSNAME###( ###CLASSNAME### const& ) = default;
    ###CLASSNAME###( ###CLASSNAME###&& )      = default;

    ###CLASSNAME###& operator= ( ###CLASSNAME### const& ) = default;
    ###CLASSNAME###& operator= ( ###CLASSNAME###&& )      = default;

    void swap( ###CLASSNAME###& other )
    {
        using std::swap;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

};

} // namespace genesis
###TEMPLATE_START###
// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "###MODULNAME###/###FILENAME###.tpp"###TEMPLATE_END###

#endif // include guard

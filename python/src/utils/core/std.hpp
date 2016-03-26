/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis;

template<class C, class T>
void python_export_function_utils_core_std_C_T ()
{
    boost::python::def(
        "contains",
        ( bool ( * )( const C &, const T & ))( &::genesis::contains ),
        ( boost::python::arg("v"), boost::python::arg("x") ),
        get_docstring("bool ::genesis::contains (const C & v, const T & x)")
    );
}

template<class Container, class UnaryPredicate>
void python_export_function_utils_core_std_Container_UnaryPredicate ()
{
    boost::python::def(
        "erase_if",
        ( void ( * )( Container &, UnaryPredicate ))( &::genesis::erase_if ),
        ( boost::python::arg("c"), boost::python::arg("p") ),
        get_docstring("void ::genesis::erase_if (Container & c, UnaryPredicate p)")
    );
}

template<typename T, typename... Args>
void python_export_function_utils_core_std_T_...Args ()
{
    boost::python::def(
        "make_unique",
        ( std::unique_ptr< T > ( * )( Args &&... ))( &::genesis::make_unique ),
        ( boost::python::arg("args") ),
        get_docstring("std::unique_ptr< T > ::genesis::make_unique (Args &&... args)")
    );
}


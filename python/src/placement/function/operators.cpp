/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS(placement_function_operators_export, "placement")
{

    boost::python::def(
        "compatible_trees",
        ( bool ( * )( const Sample &, const Sample & ))( &::genesis::placement::compatible_trees ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, Sample const & ))( &::genesis::placement::operator<< ),
        ( boost::python::arg("out"), boost::python::arg("smp") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("std::ostream & ::genesis::placement::operator<< (std::ostream & out, Sample const & smp)")
    );

    boost::python::def(
        "print_tree",
        ( std::string ( * )( Sample const & ))( &::genesis::placement::print_tree ),
        ( boost::python::arg("smp") ),
        get_docstring("std::string ::genesis::placement::print_tree (Sample const & smp)")
    );
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (NexusTrees, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusTrees
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusTrees > ( "NexusTrees", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusTrees const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_tree",
            ( void ( ::genesis::utils::NexusTrees::* )( std::string, std::string ))( &::genesis::utils::NexusTrees::add_tree ),
            ( boost::python::arg("name"), boost::python::arg("tree") )
        )
        .def(
            "block_name",
            ( std::string ( ::genesis::utils::NexusTrees::* )(  ) const )( &::genesis::utils::NexusTrees::block_name )
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::NexusTrees::* )(  ))( &::genesis::utils::NexusTrees::clear )
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::NexusTrees::* )(  ) const )( &::genesis::utils::NexusTrees::empty )
        )
        .def(
            "erase_tree",
            ( void ( ::genesis::utils::NexusTrees::* )( std::string ))( &::genesis::utils::NexusTrees::erase_tree ),
            ( boost::python::arg("name") )
        )
        .def(
            "has_tree",
            ( bool ( ::genesis::utils::NexusTrees::* )( std::string ) const )( &::genesis::utils::NexusTrees::has_tree ),
            ( boost::python::arg("name") )
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::NexusTrees::* )(  ) const )( &::genesis::utils::NexusTrees::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::NexusTrees::* )( NexusTrees & ))( &::genesis::utils::NexusTrees::swap ),
            ( boost::python::arg("other") )
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusTrees::* )( std::ostream & ) const )( &::genesis::utils::NexusTrees::to_stream ),
            ( boost::python::arg("os") )
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::NexusTrees::begin, &::genesis::utils::NexusTrees::end )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_io_nexus_trees_export, "utils")
{

    boost::python::def(
        "swap",
        ( void ( * )( NexusTrees &, NexusTrees & ))( &::genesis::utils::swap ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );
}

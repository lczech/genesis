/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::NexusTrees, scope )
{

    // -------------------------------------------------------------------
    //     Class NexusTrees
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::NexusTrees, std::shared_ptr<::genesis::utils::NexusTrees> > ( scope, "NexusTrees" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NexusTrees const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_tree",
            ( void ( ::genesis::utils::NexusTrees::* )( std::string, std::string ))( &::genesis::utils::NexusTrees::add_tree ),
            pybind11::arg("name"),
            pybind11::arg("tree")
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
            pybind11::arg("name")
        )
        .def(
            "has_tree",
            ( bool ( ::genesis::utils::NexusTrees::* )( std::string ) const )( &::genesis::utils::NexusTrees::has_tree ),
            pybind11::arg("name")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::NexusTrees::* )(  ) const )( &::genesis::utils::NexusTrees::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::NexusTrees::* )( NexusTrees & ))( &::genesis::utils::NexusTrees::swap ),
            pybind11::arg("other")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusTrees::* )( std::ostream & ) const )( &::genesis::utils::NexusTrees::to_stream ),
            pybind11::arg("os")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::NexusTrees& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_formats_nexus_trees_export, ::genesis::utils, scope )
{

    scope.def(
        "swap",
        ( void ( * )( NexusTrees &, NexusTrees & ))( &::genesis::utils::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}

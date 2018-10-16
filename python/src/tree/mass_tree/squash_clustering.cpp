/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::SquashClustering, scope )
{

    // -------------------------------------------------------------------
    //     Class SquashClustering
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::SquashClustering, std::shared_ptr<::genesis::tree::SquashClustering> > ( scope, "SquashClustering" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SquashClustering const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::tree::SquashClustering::* )(  ))( &::genesis::tree::SquashClustering::clear ),
            get_docstring("void ::genesis::tree::SquashClustering::clear ()")
        )
        .def(
            "clusters",
            ( std::vector< ::genesis::tree::SquashClustering::Cluster > const & ( ::genesis::tree::SquashClustering::* )(  ) const )( &::genesis::tree::SquashClustering::clusters )
        )
        .def(
            "mergers",
            ( std::vector< ::genesis::tree::SquashClustering::Merger > const & ( ::genesis::tree::SquashClustering::* )(  ) const )( &::genesis::tree::SquashClustering::mergers )
        )
        .def(
            "p",
            ( SquashClustering & ( ::genesis::tree::SquashClustering::* )( double ))( &::genesis::tree::SquashClustering::p ),
            pybind11::arg("value")
        )
        .def(
            "p",
            ( double ( ::genesis::tree::SquashClustering::* )(  ) const )( &::genesis::tree::SquashClustering::p )
        )
        // .def(
        //     "run",
        //     ( void ( ::genesis::tree::SquashClustering::* )( std::vector< MassTree > && ))( &::genesis::tree::SquashClustering::run ),
        //     pybind11::arg("trees"),
        //     get_docstring("void ::genesis::tree::SquashClustering::run (std::vector< MassTree > && trees)")
        // )
        .def(
            "tree_string",
            ( std::string ( ::genesis::tree::SquashClustering::* )( std::vector< std::string > const & ) const )( &::genesis::tree::SquashClustering::tree_string ),
            pybind11::arg("labels"),
            get_docstring("std::string ::genesis::tree::SquashClustering::tree_string (std::vector< std::string > const & labels) const")
        )
    ;
}

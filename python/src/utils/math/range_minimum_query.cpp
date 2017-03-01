/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::RangeMinimumQuery, scope )
{

    // -------------------------------------------------------------------
    //     Class RangeMinimumQuery
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::RangeMinimumQuery, std::shared_ptr<::genesis::utils::RangeMinimumQuery> > ( scope, "RangeMinimumQuery" )
        .def(
            pybind11::init< std::vector< ::genesis::utils::RangeMinimumQuery::IntType > const & >(),
            pybind11::arg("array")
        )
        // .def(
        //     pybind11::init< std::vector< ::genesis::utils::RangeMinimumQuery::IntType > && >(),
        //     pybind11::arg("array")
        // )
        .def(
            pybind11::init< RangeMinimumQuery const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "query",
            ( size_t ( ::genesis::utils::RangeMinimumQuery::* )( size_t, size_t ) const )( &::genesis::utils::RangeMinimumQuery::query ),
            pybind11::arg("i"),
            pybind11::arg("j")
        )
    ;
}

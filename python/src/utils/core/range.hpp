/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename IteratorType>
void PythonExportClass_::genesis::utils::Range(std::string name)
{

    // -------------------------------------------------------------------
    //     Class Range
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using RangeType = Range<typename IteratorType>;

    pybind11::class_< RangeType, std::shared_ptr<RangeType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Container & >(),
            pybind11::arg("cont")
        )
        .def(
            pybind11::init< Container const & >(),
            pybind11::arg("cont")
        )
        .def(
            pybind11::init< iterator, iterator >(),
            pybind11::arg("begin"),
            pybind11::arg("end")
        )
        .def(
            pybind11::init< Range const & >(),
            pybind11::arg("arg")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::Range& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            py::keep_alive<0, 1>()
        )
    ;
}

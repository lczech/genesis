/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename IteratorType>
void PythonExportClass_Range(std::string name)
{

    // -------------------------------------------------------------------
    //     Class Range
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using RangeType = Range<typename IteratorType>;

    boost::python::class_< RangeType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< Container & >(( boost::python::arg("cont") )) )
        .def( boost::python::init< Container const & >(( boost::python::arg("cont") )) )
        .def( boost::python::init< iterator, iterator >(( boost::python::arg("begin"), boost::python::arg("end") )) )
        .def( boost::python::init< Range const & >(( boost::python::arg("") )) )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &RangeType::begin, &RangeType::end )
        )
    ;
}

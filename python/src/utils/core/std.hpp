/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

template <typename T>
void PythonExportClass_::genesis::utils::ArrowOperatorProxy(std::string name)
{

    // -------------------------------------------------------------------
    //     Class ArrowOperatorProxy
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using ArrowOperatorProxyType = ArrowOperatorProxy<typename T>;

    pybind11::class_< ArrowOperatorProxyType, std::shared_ptr<ArrowOperatorProxyType> > ( scope, name.c_str() )
        .def(
            pybind11::init< T const & >(),
            pybind11::arg("t")
        )
    ;
}

template<typename T, typename... Args>
void python_export_function_utils_core_std_T_...Args ()
{
    scope.def(
        "make_unique",
        ( std::unique_ptr< T > ( * )( Args &&... ))( &::genesis::utils::make_unique ),
            pybind11::arg("args"),
        get_docstring("std::unique_ptr< T > ::genesis::utils::make_unique (Args &&... args)")
    );
}
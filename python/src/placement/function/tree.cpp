/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_tree_export, ::genesis::placement, scope )
{

    scope.def(
        "labelled_tree",
        ( tree::Tree ( * )( Sample const &, bool, std::string const & ))( &::genesis::placement::labelled_tree ),
            pybind11::arg("sample"),
            pybind11::arg("fully_resolve")=(bool)(false),
            pybind11::arg("name_prefix")=(std::string const &)(""),
        get_docstring("tree::Tree ::genesis::placement::labelled_tree (Sample const & sample, bool fully_resolve=false, std::string const & name_prefix=\"\")")
    );

    scope.def(
        "labelled_tree",
        ( tree::Tree ( * )( Sample const &, tree::Tree const &, bool, std::string const & ))( &::genesis::placement::labelled_tree ),
            pybind11::arg("sample"),
            pybind11::arg("tree"),
            pybind11::arg("fully_resolve")=(bool)(false),
            pybind11::arg("name_prefix")=(std::string const &)(""),
        get_docstring("tree::Tree ::genesis::placement::labelled_tree (Sample const & sample, tree::Tree const & tree, bool fully_resolve=false, std::string const & name_prefix=\"\")")
    );
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_formats_newick_parser_export, ::genesis::tree, scope )
{

    scope.def(
        "parse_newick_tree",
        ( bool ( * )( NewickLexer::iterator &, NewickLexer::iterator const &, NewickBroker & ))( &::genesis::tree::parse_newick_tree ),
            pybind11::arg("ct"),
            pybind11::arg("end"),
            pybind11::arg("broker")
    );

    scope.def(
        "generate_newick_tree",
        ( std::string ( * )( NewickBroker const & ))( &::genesis::tree::generate_newick_tree ),
            pybind11::arg("broker")
    );
}

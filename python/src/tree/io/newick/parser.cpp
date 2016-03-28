/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS(tree_io_newick_parser_export, "tree")
{

    boost::python::def(
        "parse_newick_tree",
        ( bool ( * )( NewickLexer::iterator &, const NewickLexer::iterator &, NewickBroker & ))( &::genesis::tree::parse_newick_tree ),
        ( boost::python::arg("ct"), boost::python::arg("end"), boost::python::arg("broker") ),
        get_docstring("bool ::genesis::tree::parse_newick_tree (NewickLexer::iterator & ct, const NewickLexer::iterator & end, NewickBroker & broker)")
    );

    // boost::python::def(
    //     "element_to_string",
    //     ( std::string ( * )( NewickBrokerElement const & ))( &::genesis::tree::element_to_string ),
    //     ( boost::python::arg("bn") )
    // );

    boost::python::def(
        "generate_newick_tree",
        ( std::string ( * )( NewickBroker const & ))( &::genesis::tree::generate_newick_tree ),
        ( boost::python::arg("broker") )
    );

    // boost::python::def(
    //     "to_string_rec",
    //     ( std::string ( * )( NewickBroker const &, size_t ))( &::genesis::tree::to_string_rec ),
    //     ( boost::python::arg("broker"), boost::python::arg("pos") )
    // );
}

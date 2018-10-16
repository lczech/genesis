/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::LayoutParameters, scope )
{

    // -------------------------------------------------------------------
    //     Class LayoutParameters
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::LayoutParameters, std::shared_ptr<::genesis::tree::LayoutParameters> > ( scope, "LayoutParameters" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( tree_drawing_functions_export, ::genesis::tree, scope )
{

    scope.def(
        "write_color_tree_to_nexus_file",
        ( void ( * )( CommonTree const &, std::vector< double > const &, utils::ColorMap const &, utils::ColorNormalization const &, std::string const & ))( &::genesis::tree::write_color_tree_to_nexus_file ),
            pybind11::arg("tree"),
            pybind11::arg("value_per_branch"),
            pybind11::arg("color_map"),
            pybind11::arg("color_norm"),
            pybind11::arg("nexus_filename")
    );

    scope.def(
        "write_color_tree_to_nexus_file",
        ( void ( * )( CommonTree const &, std::vector< utils::Color > const &, std::string const & ))( &::genesis::tree::write_color_tree_to_nexus_file ),
            pybind11::arg("tree"),
            pybind11::arg("color_per_branch"),
            pybind11::arg("nexus_filename")
    );

    scope.def(
        "write_color_tree_to_phyloxml_file",
        ( void ( * )( CommonTree const &, std::vector< double > const &, utils::ColorMap const &, utils::ColorNormalization const &, std::string const & ))( &::genesis::tree::write_color_tree_to_phyloxml_file ),
            pybind11::arg("tree"),
            pybind11::arg("value_per_branch"),
            pybind11::arg("color_map"),
            pybind11::arg("color_norm"),
            pybind11::arg("phyloxml_filename")
    );

    scope.def(
        "write_color_tree_to_phyloxml_file",
        ( void ( * )( CommonTree const &, std::vector< utils::Color > const &, std::string const & ))( &::genesis::tree::write_color_tree_to_phyloxml_file ),
            pybind11::arg("tree"),
            pybind11::arg("color_per_branch"),
            pybind11::arg("phyloxml_filename")
    );

    scope.def(
        "write_color_tree_to_svg_file",
        ( void ( * )( CommonTree const &, LayoutParameters const &, std::vector< double > const &, utils::ColorMap const &, utils::ColorNormalization const &, std::string const & ))( &::genesis::tree::write_color_tree_to_svg_file ),
            pybind11::arg("tree"),
            pybind11::arg("params"),
            pybind11::arg("value_per_branch"),
            pybind11::arg("color_map"),
            pybind11::arg("color_norm"),
            pybind11::arg("svg_filename")
    );

    scope.def(
        "write_color_tree_to_svg_file",
        ( void ( * )( CommonTree const &, LayoutParameters const &, std::vector< utils::Color > const &, std::string const & ))( &::genesis::tree::write_color_tree_to_svg_file ),
            pybind11::arg("tree"),
            pybind11::arg("params"),
            pybind11::arg("color_per_branch"),
            pybind11::arg("svg_filename")
    );

    scope.def(
        "write_color_tree_to_svg_file",
        ( void ( * )( CommonTree const &, LayoutParameters const &, std::vector< utils::Color > const &, utils::ColorMap const &, utils::ColorNormalization const &, std::string const & ))( &::genesis::tree::write_color_tree_to_svg_file ),
            pybind11::arg("tree"),
            pybind11::arg("params"),
            pybind11::arg("color_per_branch"),
            pybind11::arg("color_map"),
            pybind11::arg("color_norm"),
            pybind11::arg("svg_filename")
    );

    scope.def(
        "write_tree_to_newick_file",
        ( void ( * )( CommonTree const &, std::string const & ))( &::genesis::tree::write_tree_to_newick_file ),
            pybind11::arg("tree"),
            pybind11::arg("newick_filename"),
        get_docstring("void ::genesis::tree::write_tree_to_newick_file (CommonTree const & tree, std::string const & newick_filename)")
    );

    scope.def(
        "write_tree_to_nexus_file",
        ( void ( * )( CommonTree const &, std::string const & ))( &::genesis::tree::write_tree_to_nexus_file ),
            pybind11::arg("tree"),
            pybind11::arg("nexus_filename"),
        get_docstring("void ::genesis::tree::write_tree_to_nexus_file (CommonTree const & tree, std::string const & nexus_filename)")
    );

    scope.def(
        "write_tree_to_phyloxml_file",
        ( void ( * )( CommonTree const &, std::string const & ))( &::genesis::tree::write_tree_to_phyloxml_file ),
            pybind11::arg("tree"),
            pybind11::arg("phyloxml_filename"),
        get_docstring("void ::genesis::tree::write_tree_to_phyloxml_file (CommonTree const & tree, std::string const & phyloxml_filename)")
    );

    scope.def(
        "write_tree_to_svg_file",
        ( void ( * )( CommonTree const &, LayoutParameters const &, std::string const & ))( &::genesis::tree::write_tree_to_svg_file ),
            pybind11::arg("tree"),
            pybind11::arg("params"),
            pybind11::arg("svg_filename")
    );
}

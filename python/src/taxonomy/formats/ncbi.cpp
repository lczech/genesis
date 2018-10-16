/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::NcbiNode, scope )
{

    // -------------------------------------------------------------------
    //     Class NcbiNode
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::NcbiNode, std::shared_ptr<::genesis::taxonomy::NcbiNode> > ( scope, "NcbiNode" )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::NcbiName, scope )
{

    // -------------------------------------------------------------------
    //     Class NcbiName
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::NcbiName, std::shared_ptr<::genesis::taxonomy::NcbiName> > ( scope, "NcbiName" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( taxonomy_formats_ncbi_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "convert_ncbi_name_table",
        ( NcbiNameLookup ( * )( utils::CsvReader::Table const &, size_t, size_t, size_t, std::string ))( &::genesis::taxonomy::convert_ncbi_name_table ),
            pybind11::arg("name_table"),
            pybind11::arg("tax_id_pos")=(size_t)(0),
            pybind11::arg("name_pos")=(size_t)(1),
            pybind11::arg("name_class_pos")=(size_t)(3),
            pybind11::arg("name_class_filter")=(std::string)("scientific name")
    );

    scope.def(
        "convert_ncbi_node_table",
        ( NcbiNodeLookup ( * )( utils::CsvReader::Table const &, size_t, size_t, size_t ))( &::genesis::taxonomy::convert_ncbi_node_table ),
            pybind11::arg("node_table"),
            pybind11::arg("tax_id_pos")=(size_t)(0),
            pybind11::arg("parent_tax_id_pos")=(size_t)(1),
            pybind11::arg("rank_pos")=(size_t)(2)
    );

    scope.def(
        "convert_ncbi_tables",
        ( Taxonomy ( * )( NcbiNodeLookup const &, NcbiNameLookup const & ))( &::genesis::taxonomy::convert_ncbi_tables ),
            pybind11::arg("nodes"),
            pybind11::arg("names")
    );

    scope.def(
        "read_ncbi_taxonomy",
        ( Taxonomy ( * )( std::string const &, std::string const & ))( &::genesis::taxonomy::read_ncbi_taxonomy ),
            pybind11::arg("node_file"),
            pybind11::arg("name_file")
    );
}

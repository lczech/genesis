/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::TaxonomyReader, scope )
{

    // -------------------------------------------------------------------
    //     Class TaxonomyReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::TaxonomyReader, std::shared_ptr<::genesis::taxonomy::TaxonomyReader> > ( scope, "TaxonomyReader" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::taxonomy::TaxonomyReader::TaxonomyReader ()")
        )
        .def(
            pybind11::init< TaxonomyReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "csv_reader",
            ( utils::CsvReader & ( ::genesis::taxonomy::TaxonomyReader::* )(  ))( &::genesis::taxonomy::TaxonomyReader::csv_reader ),
            get_docstring("utils::CsvReader & ::genesis::taxonomy::TaxonomyReader::csv_reader ()")
        )
        .def(
            "expect_strict_order",
            ( TaxonomyReader & ( ::genesis::taxonomy::TaxonomyReader::* )( bool ))( &::genesis::taxonomy::TaxonomyReader::expect_strict_order ),
            pybind11::arg("value"),
            get_docstring("TaxonomyReader & ::genesis::taxonomy::TaxonomyReader::expect_strict_order (bool value)")
        )
        .def(
            "expect_strict_order",
            ( bool ( ::genesis::taxonomy::TaxonomyReader::* )(  ) const )( &::genesis::taxonomy::TaxonomyReader::expect_strict_order ),
            get_docstring("bool ::genesis::taxonomy::TaxonomyReader::expect_strict_order () const")
        )
        .def(
            "from_file",
            ( Taxonomy ( ::genesis::taxonomy::TaxonomyReader::* )( std::string const & ) const )( &::genesis::taxonomy::TaxonomyReader::from_file ),
            pybind11::arg("fn"),
            get_docstring("Taxonomy ::genesis::taxonomy::TaxonomyReader::from_file (std::string const & fn) const")
        )
        .def(
            "from_file",
            ( void ( ::genesis::taxonomy::TaxonomyReader::* )( std::string const &, Taxonomy & ) const )( &::genesis::taxonomy::TaxonomyReader::from_file ),
            pybind11::arg("fn"),
            pybind11::arg("tax"),
            get_docstring("void ::genesis::taxonomy::TaxonomyReader::from_file (std::string const & fn, Taxonomy & tax) const")
        )
        .def(
            "from_stream",
            ( Taxonomy ( ::genesis::taxonomy::TaxonomyReader::* )( std::istream & ) const )( &::genesis::taxonomy::TaxonomyReader::from_stream ),
            pybind11::arg("is"),
            get_docstring("Taxonomy ::genesis::taxonomy::TaxonomyReader::from_stream (std::istream & is) const")
        )
        .def(
            "from_stream",
            ( void ( ::genesis::taxonomy::TaxonomyReader::* )( std::istream &, Taxonomy & ) const )( &::genesis::taxonomy::TaxonomyReader::from_stream ),
            pybind11::arg("is"),
            pybind11::arg("tax"),
            get_docstring("void ::genesis::taxonomy::TaxonomyReader::from_stream (std::istream & is, Taxonomy & tax) const")
        )
        .def(
            "from_string",
            ( Taxonomy ( ::genesis::taxonomy::TaxonomyReader::* )( std::string const & ) const )( &::genesis::taxonomy::TaxonomyReader::from_string ),
            pybind11::arg("is"),
            get_docstring("Taxonomy ::genesis::taxonomy::TaxonomyReader::from_string (std::string const & is) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::taxonomy::TaxonomyReader::* )( std::string const &, Taxonomy & ) const )( &::genesis::taxonomy::TaxonomyReader::from_string ),
            pybind11::arg("is"),
            pybind11::arg("tax"),
            get_docstring("void ::genesis::taxonomy::TaxonomyReader::from_string (std::string const & is, Taxonomy & tax) const")
        )
        .def(
            "id_field_position",
            ( TaxonomyReader & ( ::genesis::taxonomy::TaxonomyReader::* )( int ))( &::genesis::taxonomy::TaxonomyReader::id_field_position ),
            pybind11::arg("value"),
            get_docstring("TaxonomyReader & ::genesis::taxonomy::TaxonomyReader::id_field_position (int value)")
        )
        .def(
            "id_field_position",
            ( int ( ::genesis::taxonomy::TaxonomyReader::* )(  ) const )( &::genesis::taxonomy::TaxonomyReader::id_field_position ),
            get_docstring("int ::genesis::taxonomy::TaxonomyReader::id_field_position () const")
        )
        .def(
            "name_field_position",
            ( TaxonomyReader & ( ::genesis::taxonomy::TaxonomyReader::* )( int ))( &::genesis::taxonomy::TaxonomyReader::name_field_position ),
            pybind11::arg("value"),
            get_docstring("TaxonomyReader & ::genesis::taxonomy::TaxonomyReader::name_field_position (int value)")
        )
        .def(
            "name_field_position",
            ( int ( ::genesis::taxonomy::TaxonomyReader::* )(  ) const )( &::genesis::taxonomy::TaxonomyReader::name_field_position ),
            get_docstring("int ::genesis::taxonomy::TaxonomyReader::name_field_position () const")
        )
        .def(
            "parse_document",
            ( void ( ::genesis::taxonomy::TaxonomyReader::* )( utils::InputStream &, Taxonomy & ) const )( &::genesis::taxonomy::TaxonomyReader::parse_document ),
            pybind11::arg("it"),
            pybind11::arg("tax"),
            get_docstring("void ::genesis::taxonomy::TaxonomyReader::parse_document (utils::InputStream & it, Taxonomy & tax) const")
        )
        .def(
            "parse_line",
            ( ::genesis::taxonomy::TaxonomyReader::Line ( ::genesis::taxonomy::TaxonomyReader::* )( utils::InputStream & ) const )( &::genesis::taxonomy::TaxonomyReader::parse_line ),
            pybind11::arg("it"),
            get_docstring("Line ::genesis::taxonomy::TaxonomyReader::parse_line (utils::InputStream & it) const")
        )
        .def(
            "rank_field_position",
            ( TaxonomyReader & ( ::genesis::taxonomy::TaxonomyReader::* )( int ))( &::genesis::taxonomy::TaxonomyReader::rank_field_position ),
            pybind11::arg("value"),
            get_docstring("TaxonomyReader & ::genesis::taxonomy::TaxonomyReader::rank_field_position (int value)")
        )
        .def(
            "rank_field_position",
            ( int ( ::genesis::taxonomy::TaxonomyReader::* )(  ) const )( &::genesis::taxonomy::TaxonomyReader::rank_field_position ),
            get_docstring("int ::genesis::taxonomy::TaxonomyReader::rank_field_position () const")
        )
        .def(
            "taxopath_parser",
            ( TaxopathParser & ( ::genesis::taxonomy::TaxonomyReader::* )(  ))( &::genesis::taxonomy::TaxonomyReader::taxopath_parser ),
            get_docstring("TaxopathParser & ::genesis::taxonomy::TaxonomyReader::taxopath_parser ()")
        )
    ;
}

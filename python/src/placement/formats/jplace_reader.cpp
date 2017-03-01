/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::JplaceReader, scope )
{

    // -------------------------------------------------------------------
    //     Class JplaceReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::JplaceReader, std::shared_ptr<::genesis::placement::JplaceReader> > ( scope, "JplaceReader" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< JplaceReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "from_document",
            ( Sample ( ::genesis::placement::JplaceReader::* )( utils::JsonDocument & ) const )( &::genesis::placement::JplaceReader::from_document ),
            pybind11::arg("doc"),
            get_docstring("Sample ::genesis::placement::JplaceReader::from_document (utils::JsonDocument & doc) const")
        )
        .def(
            "from_file",
            ( Sample ( ::genesis::placement::JplaceReader::* )( std::string const & ) const )( &::genesis::placement::JplaceReader::from_file ),
            pybind11::arg("fn"),
            get_docstring("Sample ::genesis::placement::JplaceReader::from_file (std::string const & fn) const")
        )
        .def(
            "from_files",
            ( SampleSet ( ::genesis::placement::JplaceReader::* )( std::vector< std::string > const & ) const )( &::genesis::placement::JplaceReader::from_files ),
            pybind11::arg("fns"),
            get_docstring("SampleSet ::genesis::placement::JplaceReader::from_files (std::vector< std::string > const & fns) const")
        )
        .def(
            "from_files",
            ( void ( ::genesis::placement::JplaceReader::* )( std::vector< std::string > const &, SampleSet & ) const )( &::genesis::placement::JplaceReader::from_files ),
            pybind11::arg("fns"),
            pybind11::arg("set"),
            get_docstring("void ::genesis::placement::JplaceReader::from_files (std::vector< std::string > const & fns, SampleSet & set) const")
        )
        .def(
            "from_stream",
            ( Sample ( ::genesis::placement::JplaceReader::* )( std::istream & ) const )( &::genesis::placement::JplaceReader::from_stream ),
            pybind11::arg("is"),
            get_docstring("Sample ::genesis::placement::JplaceReader::from_stream (std::istream & is) const")
        )
        .def(
            "from_string",
            ( Sample ( ::genesis::placement::JplaceReader::* )( std::string const & ) const )( &::genesis::placement::JplaceReader::from_string ),
            pybind11::arg("jplace"),
            get_docstring("Sample ::genesis::placement::JplaceReader::from_string (std::string const & jplace) const")
        )
        .def(
            "from_strings",
            ( SampleSet ( ::genesis::placement::JplaceReader::* )( std::vector< std::string > const & ) const )( &::genesis::placement::JplaceReader::from_strings ),
            pybind11::arg("jps"),
            get_docstring("SampleSet ::genesis::placement::JplaceReader::from_strings (std::vector< std::string > const & jps) const")
        )
        .def(
            "from_strings",
            ( void ( ::genesis::placement::JplaceReader::* )( std::vector< std::string > const &, SampleSet & ) const )( &::genesis::placement::JplaceReader::from_strings ),
            pybind11::arg("jps"),
            pybind11::arg("set"),
            get_docstring("void ::genesis::placement::JplaceReader::from_strings (std::vector< std::string > const & jps, SampleSet & set) const")
        )
        // .def(
        //     "invalid_number_behaviour",
        //     ( InvalidNumberBehaviour ( ::genesis::placement::JplaceReader::* )(  ) const )( &::genesis::placement::JplaceReader::invalid_number_behaviour ),
        //     get_docstring("InvalidNumberBehaviour ::genesis::placement::JplaceReader::invalid_number_behaviour () const")
        // )
        // .def(
        //     "invalid_number_behaviour",
        //     ( JplaceReader & ( ::genesis::placement::JplaceReader::* )( InvalidNumberBehaviour ))( &::genesis::placement::JplaceReader::invalid_number_behaviour ),
        //     pybind11::arg("val"),
        //     get_docstring("JplaceReader & ::genesis::placement::JplaceReader::invalid_number_behaviour (InvalidNumberBehaviour val)")
        // )
        .def_static(
            "check_version",
            ( bool ( * )( size_t ))( &::genesis::placement::JplaceReader::check_version ),
            pybind11::arg("version"),
            get_docstring("static bool ::genesis::placement::JplaceReader::check_version (size_t version)")
        )
        .def_static(
            "check_version",
            ( bool ( * )( std::string const & ))( &::genesis::placement::JplaceReader::check_version ),
            pybind11::arg("version"),
            get_docstring("static bool ::genesis::placement::JplaceReader::check_version (std::string const & version)")
        )
        .def_static(
            "version",
            ( std::string ( * )(  ))( &::genesis::placement::JplaceReader::version ),
            get_docstring("static std::string ::genesis::placement::JplaceReader::version ()")
        )
    ;
}

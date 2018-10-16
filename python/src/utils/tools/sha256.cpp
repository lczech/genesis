/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::SHA256, scope )
{

    // -------------------------------------------------------------------
    //     Class SHA256
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SHA256, std::shared_ptr<::genesis::utils::SHA256> > ( scope, "SHA256" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::SHA256::SHA256 ()")
        )
        .def(
            pybind11::init< SHA256 const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::utils::SHA256::* )(  ))( &::genesis::utils::SHA256::clear ),
            get_docstring("void ::genesis::utils::SHA256::clear ()")
        )
        .def(
            "final_digest",
            ( ::genesis::utils::SHA256::DigestType ( ::genesis::utils::SHA256::* )(  ))( &::genesis::utils::SHA256::final_digest ),
            get_docstring("DigestType ::genesis::utils::SHA256::final_digest ()")
        )
        .def(
            "final_hex",
            ( std::string ( ::genesis::utils::SHA256::* )(  ))( &::genesis::utils::SHA256::final_hex ),
            get_docstring("std::string ::genesis::utils::SHA256::final_hex ()")
        )
        .def(
            "update",
            ( void ( ::genesis::utils::SHA256::* )( char const *, size_t ))( &::genesis::utils::SHA256::update ),
            pybind11::arg("input"),
            pybind11::arg("length")
        )
        .def(
            "update",
            ( void ( ::genesis::utils::SHA256::* )( std::istream & ))( &::genesis::utils::SHA256::update ),
            pybind11::arg("is")
        )
        .def(
            "update",
            ( void ( ::genesis::utils::SHA256::* )( std::string const & ))( &::genesis::utils::SHA256::update ),
            pybind11::arg("s"),
            get_docstring("void ::genesis::utils::SHA256::update (std::string const & s)")
        )
        .def_static(
            "digest_to_hex",
            ( std::string ( * )( ::genesis::utils::SHA256::DigestType const & ))( &::genesis::utils::SHA256::digest_to_hex ),
            pybind11::arg("digest")
        )
        .def_static(
            "from_file_digest",
            ( ::genesis::utils::SHA256::DigestType ( * )( std::string const & ))( &::genesis::utils::SHA256::from_file_digest ),
            pybind11::arg("filename"),
            get_docstring("static DigestType ::genesis::utils::SHA256::from_file_digest (std::string const & filename)")
        )
        .def_static(
            "from_file_hex",
            ( std::string ( * )( std::string const & ))( &::genesis::utils::SHA256::from_file_hex ),
            pybind11::arg("filename"),
            get_docstring("static std::string ::genesis::utils::SHA256::from_file_hex (std::string const & filename)")
        )
        .def_static(
            "from_stream_digest",
            ( ::genesis::utils::SHA256::DigestType ( * )( std::istream & ))( &::genesis::utils::SHA256::from_stream_digest ),
            pybind11::arg("is"),
            get_docstring("static DigestType ::genesis::utils::SHA256::from_stream_digest (std::istream & is)")
        )
        .def_static(
            "from_stream_hex",
            ( std::string ( * )( std::istream & ))( &::genesis::utils::SHA256::from_stream_hex ),
            pybind11::arg("is"),
            get_docstring("static std::string ::genesis::utils::SHA256::from_stream_hex (std::istream & is)")
        )
        .def_static(
            "from_string_digest",
            ( ::genesis::utils::SHA256::DigestType ( * )( std::string const & ))( &::genesis::utils::SHA256::from_string_digest ),
            pybind11::arg("input"),
            get_docstring("static DigestType ::genesis::utils::SHA256::from_string_digest (std::string const & input)")
        )
        .def_static(
            "from_string_hex",
            ( std::string ( * )( std::string const & ))( &::genesis::utils::SHA256::from_string_hex ),
            pybind11::arg("input"),
            get_docstring("static std::string ::genesis::utils::SHA256::from_string_hex (std::string const & input)")
        )
        .def_static(
            "hex_to_digest",
            ( ::genesis::utils::SHA256::DigestType ( * )( std::string const & ))( &::genesis::utils::SHA256::hex_to_digest ),
            pybind11::arg("hex")
        )
    ;
}

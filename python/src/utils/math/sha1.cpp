/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::SHA1, scope )
{

    // -------------------------------------------------------------------
    //     Class SHA1
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SHA1, std::shared_ptr<::genesis::utils::SHA1> > ( scope, "SHA1" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SHA1 const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "final_digest",
            ( ::genesis::utils::SHA1::DigestType ( ::genesis::utils::SHA1::* )(  ))( &::genesis::utils::SHA1::final_digest )
        )
        .def(
            "final_hex",
            ( std::string ( ::genesis::utils::SHA1::* )(  ))( &::genesis::utils::SHA1::final_hex )
        )
        .def(
            "update",
            ( void ( ::genesis::utils::SHA1::* )( std::istream & ))( &::genesis::utils::SHA1::update ),
            pybind11::arg("is")
        )
        .def(
            "update",
            ( void ( ::genesis::utils::SHA1::* )( std::string const & ))( &::genesis::utils::SHA1::update ),
            pybind11::arg("s")
        )
        .def_static(
            "from_file_digest",
            ( ::genesis::utils::SHA1::DigestType ( * )( std::string const & ))( &::genesis::utils::SHA1::from_file_digest ),
            pybind11::arg("filename")
        )
        .def_static(
            "from_file_hex",
            ( std::string ( * )( std::string const & ))( &::genesis::utils::SHA1::from_file_hex ),
            pybind11::arg("filename")
        )
        .def_static(
            "from_string_digest",
            ( ::genesis::utils::SHA1::DigestType ( * )( std::string const & ))( &::genesis::utils::SHA1::from_string_digest ),
            pybind11::arg("input")
        )
        .def_static(
            "from_string_hex",
            ( std::string ( * )( std::string const & ))( &::genesis::utils::SHA1::from_string_hex ),
            pybind11::arg("input")
        )
    ;
}

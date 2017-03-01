/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename T>
void PythonExportClass_::genesis::utils::CharLookup(std::string name)
{

    // -------------------------------------------------------------------
    //     Class CharLookup
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using CharLookupType = CharLookup<typename T>;

    pybind11::class_< CharLookupType, std::shared_ptr<CharLookupType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::CharLookup::CharLookup ()")
        )
        .def(
            pybind11::init< T const & >(),
            pybind11::arg("init_all"),
            get_docstring("::genesis::utils::CharLookup::CharLookup (T const & init_all)")
        )
        .def(
            pybind11::init< std::initializer_list< T > >(),
            pybind11::arg("init"),
            get_docstring("::genesis::utils::CharLookup::CharLookup (std::initializer_list< T > init)")
        )
        .def(
            pybind11::init< CharLookup const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "all_equal_to",
            ( bool ( CharLookupType::* )( T ) const )( &CharLookupType::all_equal_to ),
            pybind11::arg("comp_value"),
            get_docstring("bool ::genesis::utils::CharLookup::all_equal_to (T comp_value) const")
        )
        .def(
            "get",
            ( T ( CharLookupType::* )( char ) const )( &CharLookupType::get ),
            pybind11::arg("c"),
            get_docstring("T ::genesis::utils::CharLookup::get (char c) const")
        )
        .def(
            "get_chars_equal_to",
            ( std::string ( CharLookupType::* )( T ) const )( &CharLookupType::get_chars_equal_to ),
            pybind11::arg("comp_value"),
            get_docstring("std::string ::genesis::utils::CharLookup::get_chars_equal_to (T comp_value) const")
        )
        .def(
            "set_all",
            ( void ( CharLookupType::* )( T ))( &CharLookupType::set_all ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::CharLookup::set_all (T value)")
        )
        .def(
            "set_char",
            ( void ( CharLookupType::* )( char, T ))( &CharLookupType::set_char ),
            pybind11::arg("c"),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::CharLookup::set_char (char c, T value)")
        )
        .def(
            "set_char_upper_lower",
            ( void ( CharLookupType::* )( char, T ))( &CharLookupType::set_char_upper_lower ),
            pybind11::arg("c"),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::CharLookup::set_char_upper_lower (char c, T value)")
        )
        .def(
            "set_if",
            ( void ( CharLookupType::* )( std::function< bool(char)>, T ))( &CharLookupType::set_if ),
            pybind11::arg("predicate"),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::CharLookup::set_if (std::function< bool(char)> predicate, T value)")
        )
        .def(
            "set_range",
            ( void ( CharLookupType::* )( char, char, T ))( &CharLookupType::set_range ),
            pybind11::arg("first"),
            pybind11::arg("last"),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::CharLookup::set_range (char first, char last, T value)")
        )
        .def(
            "set_selection",
            ( void ( CharLookupType::* )( std::string const &, T ))( &CharLookupType::set_selection ),
            pybind11::arg("chars"),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::CharLookup::set_selection (std::string const & chars, T value)")
        )
        .def(
            "set_selection_upper_lower",
            ( void ( CharLookupType::* )( std::string const &, T ))( &CharLookupType::set_selection_upper_lower ),
            pybind11::arg("chars"),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::CharLookup::set_selection_upper_lower (std::string const & chars, T value)")
        )

        // Operators

        .def(
            "__getitem__",
            ( T ( CharLookupType::* )( char ) const )( &CharLookupType::operator[] ),
            pybind11::arg("c"),
            get_docstring("T ::genesis::utils::CharLookup::operator[] (char c) const")
        )
    ;
}

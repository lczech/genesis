/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS( sequence_functions_codes_export, ::genesis::sequence, scope )
{

    scope.def(
        "normalize_amino_acid_code",
        ( char ( * )( char, bool ))( &::genesis::sequence::normalize_amino_acid_code ),
            pybind11::arg("code"),
            pybind11::arg("accept_degenerated")=(bool)(true),
        get_docstring("char ::genesis::sequence::normalize_amino_acid_code (char code, bool accept_degenerated=true)")
    );

    scope.def(
        "normalize_nucleic_acid_code",
        ( char ( * )( char, bool ))( &::genesis::sequence::normalize_nucleic_acid_code ),
            pybind11::arg("code"),
            pybind11::arg("accept_degenerated")=(bool)(true),
        get_docstring("char ::genesis::sequence::normalize_nucleic_acid_code (char code, bool accept_degenerated=true)")
    );

    scope.def(
        "nucleic_acid_ambiguity_code",
        ( char ( * )( std::string ))( &::genesis::sequence::nucleic_acid_ambiguity_code ),
            pybind11::arg("codes"),
        get_docstring("char ::genesis::sequence::nucleic_acid_ambiguity_code (std::string codes)")
    );

    scope.def(
        "amino_acid_text_colors",
        ( std::map< char, std::string > ( * )(  ))( &::genesis::sequence::amino_acid_text_colors ),
        get_docstring("std::map< char, std::string > ::genesis::sequence::amino_acid_text_colors ()")
    );

    scope.def(
        "nucleic_acid_text_colors",
        ( std::map< char, std::string > ( * )(  ))( &::genesis::sequence::nucleic_acid_text_colors ),
        get_docstring("std::map< char, std::string > ::genesis::sequence::nucleic_acid_text_colors ()")
    );

    scope.def(
        "amino_acid_colors",
        ( std::map< char, utils::Color > ( * )(  ))( &::genesis::sequence::amino_acid_colors ),
        get_docstring("std::map< char, utils::Color > ::genesis::sequence::amino_acid_colors ()")
    );

    scope.def(
        "nucleic_acid_colors",
        ( std::map< char, utils::Color > ( * )(  ))( &::genesis::sequence::nucleic_acid_colors ),
        get_docstring("std::map< char, utils::Color > ::genesis::sequence::nucleic_acid_colors ()")
    );

    scope.def(
        "amino_acid_codes_all",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_all ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_all ()")
    );

    scope.def(
        "amino_acid_codes_degenerated",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_degenerated ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_degenerated ()")
    );

    scope.def(
        "amino_acid_codes_plain",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_plain ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_plain ()")
    );

    scope.def(
        "amino_acid_codes_undetermined",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_undetermined ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_undetermined ()")
    );

    scope.def(
        "amino_acid_name",
        ( std::string ( * )( char ))( &::genesis::sequence::amino_acid_name ),
            pybind11::arg("code"),
        get_docstring("std::string ::genesis::sequence::amino_acid_name (char code)")
    );

    scope.def(
        "normalize_code_alphabet",
        ( std::string ( * )( std::string const & ))( &::genesis::sequence::normalize_code_alphabet ),
            pybind11::arg("alphabet"),
        get_docstring("std::string ::genesis::sequence::normalize_code_alphabet (std::string const & alphabet)")
    );

    scope.def(
        "nucleic_acid_ambiguities",
        ( std::string ( * )( char ))( &::genesis::sequence::nucleic_acid_ambiguities ),
            pybind11::arg("code"),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_ambiguities (char code)")
    );

    scope.def(
        "nucleic_acid_codes_all",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_all ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_all ()")
    );

    scope.def(
        "nucleic_acid_codes_degenerated",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_degenerated ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_degenerated ()")
    );

    scope.def(
        "nucleic_acid_codes_plain",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_plain ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_plain ()")
    );

    scope.def(
        "nucleic_acid_codes_undetermined",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_undetermined ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_undetermined ()")
    );

    scope.def(
        "nucleic_acid_name",
        ( std::string ( * )( char ))( &::genesis::sequence::nucleic_acid_name ),
            pybind11::arg("code"),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_name (char code)")
    );

    scope.def(
        "reverse_complement",
        ( std::string ( * )( std::string const &, bool ))( &::genesis::sequence::reverse_complement ),
            pybind11::arg("sequence"),
            pybind11::arg("accept_degenerated")=(bool)(true),
        get_docstring("std::string ::genesis::sequence::reverse_complement (std::string const & sequence, bool accept_degenerated=true)")
    );
}
